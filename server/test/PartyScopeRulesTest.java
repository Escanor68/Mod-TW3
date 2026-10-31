public class PartyScopeRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        expectEquals("alice", PartyScopeRules.partyKey(false, "", "Alice"));
        expectEquals("alice", PartyScopeRules.partyKey(true, "Alice", "Bob"));
        expectEquals("bob", PartyScopeRules.partyKey(true, "none", "Bob"));

        expectTrue(PartyScopeRules.sameParty(false, "", "Alice", true, "Alice", "Bob"));
        expectTrue(PartyScopeRules.sameParty(true, "Alice", "Carol", true, "Alice", "Bob"));
        expectFalse(PartyScopeRules.sameParty(false, "", "Alice", false, "", "Bob"));

        expectTrue(PartyScopeRules.parseTruthy("true"));
        expectFalse(PartyScopeRules.parseTruthy("false"));

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures);
            System.exit(1);
        }

        System.out.println("PartyScopeRulesTest: PASS");
    }

    private static void expectEquals(String expected, String actual)
    {
        if (expected == null ? actual != null : !expected.equals(actual))
        {
            System.err.println("expectedEquals failed: " + expected + " vs " + actual);
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
