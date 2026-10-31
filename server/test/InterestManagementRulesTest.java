public class InterestManagementRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        float r = InterestManagementRules.DEFAULT_INTEREST_RADIUS;

        expectTrue(InterestManagementRules.withinInterest(true, true, 0, 0, 0, 50, 0, 0, r));
        expectFalse(InterestManagementRules.withinInterest(true, true, 0, 0, 0, 200, 0, 0, r));
        expectTrue(InterestManagementRules.withinInterest(false, true, 0, 0, 0, 200, 0, 0, r));

        expectTrue(InterestManagementRules.shouldRelay(
                true, true, true, 0, 0, 0, 500, 0, 0, r));
        expectFalse(InterestManagementRules.shouldRelay(
                false, true, true, 0, 0, 0, 500, 0, 0, r));
        expectTrue(InterestManagementRules.shouldRelay(
                false, true, true, 0, 0, 0, 10, 0, 0, r));

        expectEquals(1, InterestManagementRules.maxRecipientsWhenIsolated(50));
        expectEquals(50, InterestManagementRules.baselineGlobalRecipients(50));
        expectEquals(0, InterestManagementRules.maxRecipientsWhenIsolated(0));

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures);
            System.exit(1);
        }

        System.out.println("InterestManagementRulesTest: PASS");
    }

    private static void expectEquals(int expected, int actual)
    {
        if (expected != actual)
        {
            System.err.println("expectEquals failed: " + expected + " vs " + actual);
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
