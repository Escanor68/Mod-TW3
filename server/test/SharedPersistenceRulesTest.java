import java.util.LinkedHashMap;
import java.util.Map;

public class SharedPersistenceRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        expectEquals(0L, SharedPersistenceRules.clampCrowns(-5L));
        expectEquals(SharedPersistenceRules.MAX_CROWNS, SharedPersistenceRules.clampCrowns(SharedPersistenceRules.MAX_CROWNS + 10L));
        expectEquals(42L, SharedPersistenceRules.clampCrowns(42L));

        expectEquals(0, SharedPersistenceRules.clampQty(-1));
        expectEquals(SharedPersistenceRules.MAX_ITEM_QTY, SharedPersistenceRules.clampQty(5000));

        expectEquals("White+Honey", SharedPersistenceRules.toWireItemName("White Honey"));
        expectEquals("White Honey", SharedPersistenceRules.fromWireItemName("White+Honey"));
        expectEquals("", SharedPersistenceRules.sanitizeWireItemName("!!!"));

        Map<String, Integer> parsed = SharedPersistenceRules.parseBagBlob("Swallow=2;Swallow=5;Bad;White+Honey=3");
        expectEquals(2, parsed.size());
        expectEquals(5, parsed.get("Swallow").intValue()); // overwrite, not sum
        expectEquals(3, parsed.get("White+Honey").intValue());

        Map<String, Integer> empty = SharedPersistenceRules.parseBagBlob("-");
        expectTrue(empty.isEmpty());

        String formatted = SharedPersistenceRules.formatBagBlob(parsed);
        expectTrue(formatted.contains("Swallow=5"));
        expectTrue(formatted.contains("White+Honey=3"));

        Map<String, Integer> localInflated = new LinkedHashMap<>();
        localInflated.put("Swallow", 99);
        Map<String, Integer> auth = SharedPersistenceRules.authoritativeCoveredBag(parsed);
        expectEquals(5, auth.get("Swallow").intValue());
        expectFalse(auth.containsKey("LocalOnly"));

        // Cap bag entries
        StringBuilder huge = new StringBuilder();
        for (int i = 0; i < 20; i++)
        {
            if (i > 0)
            {
                huge.append(';');
            }
            huge.append("Item").append(i).append('=').append(1);
        }
        expectEquals(SharedPersistenceRules.MAX_BAG_ENTRIES, SharedPersistenceRules.parseBagBlob(huge.toString()).size());

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures);
            System.exit(1);
        }

        System.out.println("SharedPersistenceRulesTest: PASS");
    }

    private static void expectEquals(long expected, long actual)
    {
        if (expected != actual)
        {
            System.err.println("expectEquals long failed: " + expected + " vs " + actual);
            failures++;
        }
    }

    private static void expectEquals(int expected, int actual)
    {
        if (expected != actual)
        {
            System.err.println("expectEquals int failed: " + expected + " vs " + actual);
            failures++;
        }
    }

    private static void expectEquals(String expected, String actual)
    {
        if (expected == null ? actual != null : !expected.equals(actual))
        {
            System.err.println("expectEquals string failed: " + expected + " vs " + actual);
            failures++;
        }
    }

    private static void expectTrue(boolean v)
    {
        if (!v)
        {
            System.err.println("expectedTrue failed");
            failures++;
        }
    }

    private static void expectFalse(boolean v)
    {
        if (v)
        {
            System.err.println("expectedFalse failed");
            failures++;
        }
    }
}
