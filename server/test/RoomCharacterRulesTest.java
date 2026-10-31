import java.util.Arrays;
import java.util.Collections;

public class RoomCharacterRulesTest
{
    private static int failures = 0;

    public static void main(String[] args)
    {
        expectTrue(RoomCharacterRules.isOwnerSpecies("geralt"));
        expectTrue(RoomCharacterRules.isStorySupportSpecies("HUMAN"));
        expectTrue(RoomCharacterRules.isStorySupportSpecies("elf"));
        expectTrue(RoomCharacterRules.isStorySupportSpecies("Witcher"));
        expectTrue(RoomCharacterRules.isStorySupportSpecies("SORCERESS"));
        expectFalse(RoomCharacterRules.isStorySupportSpecies("GERALT"));
        expectFalse(RoomCharacterRules.isStorySupportSpecies("EREDIN"));

        expectTrue(RoomCharacterRules.isAllowedSpecies(RoomCharacterRules.RoomMode.STORY, "ELF"));
        expectFalse(RoomCharacterRules.isAllowedSpecies(RoomCharacterRules.RoomMode.STORY, "EREDIN"));
        expectTrue(RoomCharacterRules.isAllowedSpecies(RoomCharacterRules.RoomMode.CONTRACTS, "EREDIN"));
        expectTrue(RoomCharacterRules.isAllowedSpecies(RoomCharacterRules.RoomMode.CONTRACTS, "EMHYR"));
        expectFalse(RoomCharacterRules.isAllowedSpecies(RoomCharacterRules.RoomMode.CONTRACTS, ""));

        expectEq(RoomCharacterRules.CPC_GERALT, RoomCharacterRules.cpcNameForStorySpecies("GERALT"));
        expectEq(RoomCharacterRules.CPC_WITCHER, RoomCharacterRules.cpcNameForStorySpecies("WITCHER"));
        expectEq(RoomCharacterRules.CPC_SORCERESS, RoomCharacterRules.cpcNameForStorySpecies("SORCERESS"));
        expectEq(RoomCharacterRules.CPC_HUMAN, RoomCharacterRules.cpcNameForStorySpecies("HUMAN"));
        expectEq(RoomCharacterRules.CPC_ELF, RoomCharacterRules.cpcNameForStorySpecies("ELF"));

        expectFalse(RoomCharacterRules.midStoryContractCompletionSwitchesMode());

        expectTrue(RoomCharacterRules.canDeclareHistoriaTerminada(
                RoomCharacterRules.RoomMode.STORY, true));
        expectFalse(RoomCharacterRules.canDeclareHistoriaTerminada(
                RoomCharacterRules.RoomMode.STORY, false));
        expectFalse(RoomCharacterRules.canDeclareHistoriaTerminada(
                RoomCharacterRules.RoomMode.CONTRACTS, true));
        expectEq("CONTRACTS", RoomCharacterRules.modeWire(RoomCharacterRules.modeAfterHistoriaTerminada()));

        expectTrue(RoomCharacterRules.canClaimStoryOwner(false, "GERALT"));
        expectFalse(RoomCharacterRules.canClaimStoryOwner(true, "GERALT"));
        expectTrue(RoomCharacterRules.shouldRejectSecondGeraltOwner(true, "GERALT"));
        expectFalse(RoomCharacterRules.shouldRejectSecondGeraltOwner(false, "GERALT"));
        expectFalse(RoomCharacterRules.shouldRejectSecondGeraltOwner(true, "ELF"));

        expectEq("", RoomCharacterRules.pickHighestLevelUsername(Collections.emptyList()));
        expectEq("Bob", RoomCharacterRules.pickHighestLevelUsername(Arrays.asList(
                new RoomCharacterRules.MemberLevel("Alice", 10),
                new RoomCharacterRules.MemberLevel("Bob", 40),
                new RoomCharacterRules.MemberLevel("Cara", 40)
        )));
        // tie on level → username ascending
        expectEq("Alice", RoomCharacterRules.pickHighestLevelUsername(Arrays.asList(
                new RoomCharacterRules.MemberLevel("Bob", 5),
                new RoomCharacterRules.MemberLevel("Alice", 5)
        )));

        // Owner may be lower level than supports and still be Geralt (selection is leave-only)
        expectTrue(RoomCharacterRules.isOwnerSpecies("GERALT"));

        expectEq(12, RoomCharacterRules.parseLevel("12", 0));
        expectEq(0, RoomCharacterRules.parseLevel("x", 0));

        if (failures > 0)
        {
            System.err.println("FAILED: " + failures);
            System.exit(1);
        }

        System.out.println("RoomCharacterRulesTest: PASS");
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

    private static void expectEq(String expected, String actual)
    {
        if (expected == null ? actual != null : !expected.equals(actual))
        {
            System.err.println("expectedEq failed: expected=" + expected + " actual=" + actual);
            failures++;
        }
    }

    private static void expectEq(int expected, int actual)
    {
        if (expected != actual)
        {
            System.err.println("expectedEq failed: expected=" + expected + " actual=" + actual);
            failures++;
        }
    }
}
