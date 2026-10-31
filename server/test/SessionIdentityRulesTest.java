/**
 * Lightweight checks (no JUnit dependency). Run via server/scripts/run-identity-tests.sh
 */
public class SessionIdentityRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        expectEquals("alice", SessionIdentityRules.normalizeUsernameKey("  Alice  "));
        expectEquals("", SessionIdentityRules.normalizeUsernameKey(null));
        expectEquals("", SessionIdentityRules.normalizeUsernameKey("   "));

        expectTrue(SessionIdentityRules.isReservationActive(100L, 200L));
        expectFalse(SessionIdentityRules.isReservationActive(200L, 200L));
        expectFalse(SessionIdentityRules.isReservationActive(201L, 200L));

        expectTrue(SessionIdentityRules.canClaimReservedUsername("1.2.3.4", "1.2.3.4", 100L, 200L));
        expectFalse(SessionIdentityRules.canClaimReservedUsername("1.2.3.4", "9.9.9.9", 100L, 200L));
        expectTrue(SessionIdentityRules.canClaimReservedUsername("1.2.3.4", "9.9.9.9", 250L, 200L));

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures + " assertion(s)");
            System.exit(1);
        }

        System.out.println("SessionIdentityRulesTest: PASS");
    }

    private static void expectEquals(String expected, String actual)
    {
        if (expected == null ? actual != null : !expected.equals(actual))
        {
            System.err.println("expectedEquals failed: expected=[" + expected + "] actual=[" + actual + "]");
            failures++;
        }
    }

    private static void expectTrue(boolean value)
    {
        if (!value)
        {
            System.err.println("expectedTrue failed");
            failures++;
        }
    }

    private static void expectFalse(boolean value)
    {
        if (value)
        {
            System.err.println("expectedFalse failed");
            failures++;
        }
    }
}
