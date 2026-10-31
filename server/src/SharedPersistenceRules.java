import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

/**
 * Pure shared-persistence helpers for F5 (crowns + small key-item bag).
 * Conflict rule: server snapshot replaces the covered slice (no additive merge).
 */
public final class SharedPersistenceRules
{
    public static final long MAX_CROWNS = 999_999_999L;
    public static final int MAX_BAG_ENTRIES = 12;
    public static final int MAX_ITEM_QTY = 999;
    public static final int MAX_ITEM_NAME_LEN = 48;
    public static final String EMPTY_BAG = "-";

    private SharedPersistenceRules()
    {
    }

    public static long clampCrowns(long requested)
    {
        if (requested < 0L)
        {
            return 0L;
        }

        return Math.min(requested, MAX_CROWNS);
    }

    public static int clampQty(int requested)
    {
        if (requested < 0)
        {
            return 0;
        }

        return Math.min(requested, MAX_ITEM_QTY);
    }

    /**
     * Wire names: letters, digits, {@code +} (space), underscore, hyphen.
     */
    public static String sanitizeWireItemName(String raw)
    {
        if (raw == null)
        {
            return "";
        }

        String trimmed = raw.trim();
        if (trimmed.isEmpty())
        {
            return "";
        }

        StringBuilder out = new StringBuilder(trimmed.length());
        for (int i = 0; i < trimmed.length() && out.length() < MAX_ITEM_NAME_LEN; i++)
        {
            char c = trimmed.charAt(i);
            if ((c >= 'a' && c <= 'z')
                    || (c >= 'A' && c <= 'Z')
                    || (c >= '0' && c <= '9')
                    || c == '+'
                    || c == '_'
                    || c == '-')
            {
                out.append(c);
            }
        }

        return out.toString();
    }

    public static String toWireItemName(String scriptName)
    {
        if (scriptName == null)
        {
            return "";
        }

        return sanitizeWireItemName(scriptName.trim().replace(' ', '+'));
    }

    public static String fromWireItemName(String wireName)
    {
        String clean = sanitizeWireItemName(wireName);
        if (clean.isEmpty())
        {
            return "";
        }

        return clean.replace('+', ' ');
    }

    /**
     * Parse {@code Name=qty;Name=qty}. Later entries with the same name overwrite earlier ones
     * (server-wins / anti-dupe: never sum quantities).
     */
    public static Map<String, Integer> parseBagBlob(String blob)
    {
        Map<String, Integer> out = new LinkedHashMap<>();
        if (blob == null)
        {
            return out;
        }

        String trimmed = blob.trim();
        if (trimmed.isEmpty() || EMPTY_BAG.equals(trimmed) || "none".equalsIgnoreCase(trimmed))
        {
            return out;
        }

        String[] entries = trimmed.split(";");
        for (String entry : entries)
        {
            if (out.size() >= MAX_BAG_ENTRIES)
            {
                break;
            }

            if (entry == null || entry.isEmpty())
            {
                continue;
            }

            int eq = entry.indexOf('=');
            if (eq <= 0 || eq >= entry.length() - 1)
            {
                continue;
            }

            String name = sanitizeWireItemName(entry.substring(0, eq));
            if (name.isEmpty())
            {
                continue;
            }

            int qty;
            try
            {
                qty = clampQty(Integer.parseInt(entry.substring(eq + 1).trim()));
            }
            catch (NumberFormatException e)
            {
                continue;
            }

            if (qty <= 0)
            {
                out.remove(name);
                continue;
            }

            // Overwrite, never add — prevents client-side qty inflation on re-save.
            out.put(name, qty);
        }

        return out;
    }

    public static String formatBagBlob(Map<String, Integer> bag)
    {
        if (bag == null || bag.isEmpty())
        {
            return EMPTY_BAG;
        }

        StringBuilder sb = new StringBuilder();
        int count = 0;
        for (Map.Entry<String, Integer> entry : bag.entrySet())
        {
            if (count >= MAX_BAG_ENTRIES)
            {
                break;
            }

            String name = sanitizeWireItemName(entry.getKey());
            int qty = clampQty(entry.getValue() == null ? 0 : entry.getValue());
            if (name.isEmpty() || qty <= 0)
            {
                continue;
            }

            if (sb.length() > 0)
            {
                sb.append(';');
            }
            sb.append(name).append('=').append(qty);
            count++;
        }

        return sb.length() == 0 ? EMPTY_BAG : sb.toString();
    }

    /**
     * Server-wins replace: covered names become exactly {@code serverBag}; other local names untouched.
     * Returns the authoritative bag to apply for covered names (qty 0 means remove all).
     */
    public static Map<String, Integer> authoritativeCoveredBag(Map<String, Integer> serverBag)
    {
        if (serverBag == null || serverBag.isEmpty())
        {
            return Collections.emptyMap();
        }

        Map<String, Integer> out = new LinkedHashMap<>();
        for (Map.Entry<String, Integer> entry : serverBag.entrySet())
        {
            String name = sanitizeWireItemName(entry.getKey());
            if (name.isEmpty())
            {
                continue;
            }
            out.put(name, clampQty(entry.getValue() == null ? 0 : entry.getValue()));
        }
        return out;
    }

    public static List<String> sortedWireNames(Map<String, Integer> bag)
    {
        List<String> names = new ArrayList<>(bag.keySet());
        Collections.sort(names, String.CASE_INSENSITIVE_ORDER);
        return names;
    }

    public static String normalizeUsernameKey(String username)
    {
        return username == null ? "" : username.trim().toLowerCase(Locale.ROOT);
    }
}
