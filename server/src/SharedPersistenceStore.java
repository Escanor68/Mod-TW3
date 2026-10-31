import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Line-oriented player snapshot store (F5).
 * Format per line: {@code usernameKey\tcrowns\tbagBlob\tupdatedAtMs}
 */
public final class SharedPersistenceStore
{
    public static final class Snapshot
    {
        public final long crowns;
        public final String bagBlob;
        public final long updatedAtMs;

        public Snapshot(long crowns, String bagBlob, long updatedAtMs)
        {
            this.crowns = crowns;
            this.bagBlob = bagBlob == null || bagBlob.isEmpty()
                    ? SharedPersistenceRules.EMPTY_BAG
                    : bagBlob;
            this.updatedAtMs = updatedAtMs;
        }
    }

    private final Path path;
    private final ConcurrentHashMap<String, Snapshot> snapshots = new ConcurrentHashMap<>();

    public SharedPersistenceStore(Path path)
    {
        this.path = path;
    }

    public void load()
    {
        snapshots.clear();
        if (path == null || !Files.exists(path))
        {
            return;
        }

        try
        {
            List<String> lines = Files.readAllLines(path, StandardCharsets.UTF_8);
            for (String line : lines)
            {
                if (line == null)
                {
                    continue;
                }

                String trimmed = line.trim();
                if (trimmed.isEmpty() || trimmed.startsWith("#"))
                {
                    continue;
                }

                String[] parts = trimmed.split("\t", -1);
                if (parts.length < 3)
                {
                    continue;
                }

                String key = SharedPersistenceRules.normalizeUsernameKey(parts[0]);
                if (key.isEmpty())
                {
                    continue;
                }

                long crowns;
                try
                {
                    crowns = SharedPersistenceRules.clampCrowns(Long.parseLong(parts[1].trim()));
                }
                catch (NumberFormatException e)
                {
                    continue;
                }

                Map<String, Integer> bag = SharedPersistenceRules.parseBagBlob(parts[2]);
                String bagBlob = SharedPersistenceRules.formatBagBlob(bag);
                long updatedAt = 0L;
                if (parts.length >= 4)
                {
                    try
                    {
                        updatedAt = Long.parseLong(parts[3].trim());
                    }
                    catch (NumberFormatException ignored)
                    {
                        updatedAt = 0L;
                    }
                }

                snapshots.put(key, new Snapshot(crowns, bagBlob, updatedAt));
            }
        }
        catch (IOException e)
        {
            // Caller logs; keep empty map.
        }
    }

    public Snapshot get(String usernameKey)
    {
        if (usernameKey == null || usernameKey.isEmpty())
        {
            return null;
        }

        return snapshots.get(usernameKey);
    }

    public Snapshot putReplace(String usernameKey, long crowns, String bagBlob)
    {
        String key = SharedPersistenceRules.normalizeUsernameKey(usernameKey);
        if (key.isEmpty())
        {
            return null;
        }

        long safeCrowns = SharedPersistenceRules.clampCrowns(crowns);
        Map<String, Integer> bag = SharedPersistenceRules.parseBagBlob(bagBlob);
        String safeBag = SharedPersistenceRules.formatBagBlob(bag);
        Snapshot snap = new Snapshot(safeCrowns, safeBag, System.currentTimeMillis());
        snapshots.put(key, snap);
        flush();
        return snap;
    }

    public int size()
    {
        return snapshots.size();
    }

    public void flush()
    {
        if (path == null)
        {
            return;
        }

        try
        {
            if (path.getParent() != null)
            {
                Files.createDirectories(path.getParent());
            }

            List<String> keys = new ArrayList<>(snapshots.keySet());
            Collections.sort(keys);

            StringBuilder sb = new StringBuilder();
            sb.append("# WitcherOnline shared persistence (F5)\n");
            sb.append("# usernameKey\tcrowns\tbagBlob\tupdatedAtMs\n");
            for (String key : keys)
            {
                Snapshot snap = snapshots.get(key);
                if (snap == null)
                {
                    continue;
                }
                sb.append(key)
                        .append('\t')
                        .append(snap.crowns)
                        .append('\t')
                        .append(snap.bagBlob)
                        .append('\t')
                        .append(snap.updatedAtMs)
                        .append('\n');
            }

            Files.writeString(path, sb.toString(), StandardCharsets.UTF_8);
        }
        catch (IOException e)
        {
            // Caller may log separately.
        }
    }
}
