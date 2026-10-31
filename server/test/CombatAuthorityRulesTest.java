public class CombatAuthorityRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        expectEquals(0.35f, CombatAuthorityRules.clampDamage(0.9f), 0.0001f);
        expectEquals(0.0f, CombatAuthorityRules.clampDamage(0.001f), 0.0001f);
        expectEquals(0.0f, CombatAuthorityRules.clampDamage(Float.NaN), 0.0001f);

        expectEquals(0.7f, CombatAuthorityRules.applyDamage(1.0f, 0.3f), 0.0001f);
        expectEquals(0.0f, CombatAuthorityRules.applyDamage(0.1f, 0.3f), 0.0001f);

        expectTrue(CombatAuthorityRules.cooldownElapsed(1_000_000_000L, 0L));
        expectFalse(CombatAuthorityRules.cooldownElapsed(1_000_000_000L, 900_000_000L));
        expectTrue(CombatAuthorityRules.cooldownElapsed(1_200_000_000L, 900_000_000L));

        expectTrue(CombatAuthorityRules.withinHitRange(true, true, 0, 0, 0, 3, 0, 0));
        expectFalse(CombatAuthorityRules.withinHitRange(true, true, 0, 0, 0, 20, 0, 0));
        expectTrue(CombatAuthorityRules.withinHitRange(false, true, 0, 0, 0, 20, 0, 0));

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures);
            System.exit(1);
        }

        System.out.println("CombatAuthorityRulesTest: PASS");
    }

    private static void expectEquals(float expected, float actual, float eps)
    {
        if (Math.abs(expected - actual) > eps)
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
