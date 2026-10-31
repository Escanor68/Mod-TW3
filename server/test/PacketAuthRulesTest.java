public class PacketAuthRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        expectFalse(PacketAuthRules.isRequired(null));
        expectFalse(PacketAuthRules.isRequired(""));
        expectFalse(PacketAuthRules.isRequired("  "));
        expectTrue(PacketAuthRules.isRequired("secret"));

        expectTrue(PacketAuthRules.matches("", "anything"));
        expectTrue(PacketAuthRules.matches("secret", "secret"));
        expectFalse(PacketAuthRules.matches("secret", "other"));
        expectFalse(PacketAuthRules.matches("secret", null));

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures);
            System.exit(1);
        }

        System.out.println("PacketAuthRulesTest: PASS");
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
