import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Locale;

/**
 * Story-room character / mode rules (F7).
 * Story: one Geralt owner + support species. Contracts: free pick after "historia terminada".
 */
public final class RoomCharacterRules
{
    public enum RoomMode
    {
        STORY,
        CONTRACTS
    }

    public static final String SPECIES_GERALT = "GERALT";
    public static final String SPECIES_HUMAN = "HUMAN";
    public static final String SPECIES_ELF = "ELF";
    public static final String SPECIES_WITCHER = "WITCHER";
    public static final String SPECIES_SORCERESS = "SORCERESS";

    /** Canonical CPC names known in-repo scripts (ordinals live in external CPC mod). */
    public static final String CPC_GERALT = "ENR_PlayerGeralt";
    public static final String CPC_WITCHER = "ENR_PlayerWitcher";
    public static final String CPC_SORCERESS = "ENR_PlayerSorceress";
    public static final String CPC_HUMAN = "ENR_PlayerHuman";
    public static final String CPC_ELF = "ENR_PlayerElf";

    private RoomCharacterRules()
    {
    }

    public static String normalizeSpecies(String species)
    {
        return species == null ? "" : species.trim().toUpperCase(Locale.ROOT);
    }

    public static boolean isOwnerSpecies(String species)
    {
        return SPECIES_GERALT.equals(normalizeSpecies(species));
    }

    public static boolean isStorySupportSpecies(String species)
    {
        String s = normalizeSpecies(species);
        return SPECIES_HUMAN.equals(s)
                || SPECIES_ELF.equals(s)
                || SPECIES_WITCHER.equals(s)
                || SPECIES_SORCERESS.equals(s);
    }

    /**
     * Story-mode cast gate. Contracts mode allows any non-empty species token (free pick).
     */
    public static boolean isAllowedSpecies(RoomMode mode, String species)
    {
        String s = normalizeSpecies(species);
        if (s.isEmpty())
        {
            return false;
        }

        if (mode == RoomMode.CONTRACTS)
        {
            return true;
        }

        return isOwnerSpecies(s) || isStorySupportSpecies(s);
    }

    public static String cpcNameForStorySpecies(String species)
    {
        String s = normalizeSpecies(species);
        switch (s)
        {
            case SPECIES_GERALT:
                return CPC_GERALT;
            case SPECIES_WITCHER:
                return CPC_WITCHER;
            case SPECIES_SORCERESS:
                return CPC_SORCERESS;
            case SPECIES_HUMAN:
                return CPC_HUMAN;
            case SPECIES_ELF:
                return CPC_ELF;
            default:
                return "";
        }
    }

    public static boolean midStoryContractCompletionSwitchesMode()
    {
        return false;
    }

    public static boolean canDeclareHistoriaTerminada(RoomMode mode, boolean isRoomOwner)
    {
        return mode == RoomMode.STORY && isRoomOwner;
    }

    public static RoomMode modeAfterHistoriaTerminada()
    {
        return RoomMode.CONTRACTS;
    }

    /**
     * First Geralt in a story room may claim owner. A second Geralt claim is rejected
     * (caller demotes to support or ERROR).
     */
    public static boolean canClaimStoryOwner(boolean roomAlreadyHasOwner, String species)
    {
        return isOwnerSpecies(species) && !roomAlreadyHasOwner;
    }

    /**
     * In story mode, claiming GERALT while another owner exists is not allowed as owner.
     */
    public static boolean shouldRejectSecondGeraltOwner(boolean roomAlreadyHasOwner, String species)
    {
        return roomAlreadyHasOwner && isOwnerSpecies(species);
    }

    public static final class MemberLevel
    {
        public final String username;
        public final int level;

        public MemberLevel(String username, int level)
        {
            this.username = username == null ? "" : username;
            this.level = level;
        }
    }

    /**
     * Highest level wins; ties break by case-insensitive username ascending (stable).
     * Empty/blank usernames are ignored. Returns "" if no candidates.
     */
    public static String pickHighestLevelUsername(List<MemberLevel> members)
    {
        if (members == null || members.isEmpty())
        {
            return "";
        }

        List<MemberLevel> copy = new ArrayList<>();
        for (MemberLevel m : members)
        {
            if (m == null || m.username == null || m.username.trim().isEmpty())
            {
                continue;
            }
            copy.add(m);
        }

        if (copy.isEmpty())
        {
            return "";
        }

        Collections.sort(copy, (a, b) -> {
            int byLevel = Integer.compare(b.level, a.level);
            if (byLevel != 0)
            {
                return byLevel;
            }
            return a.username.trim().toLowerCase(Locale.ROOT)
                    .compareTo(b.username.trim().toLowerCase(Locale.ROOT));
        });

        return copy.get(0).username;
    }

    public static int parseLevel(String raw, int fallback)
    {
        if (raw == null || raw.trim().isEmpty())
        {
            return fallback;
        }

        try
        {
            return Integer.parseInt(raw.trim());
        }
        catch (NumberFormatException e)
        {
            return fallback;
        }
    }

    public static RoomMode parseMode(String raw)
    {
        if (raw == null)
        {
            return RoomMode.STORY;
        }

        String s = raw.trim().toUpperCase(Locale.ROOT);
        if ("CONTRACTS".equals(s) || "CONTRACT".equals(s) || "EXPLORE".equals(s))
        {
            return RoomMode.CONTRACTS;
        }

        return RoomMode.STORY;
    }

    public static String modeWire(RoomMode mode)
    {
        return mode == RoomMode.CONTRACTS ? "CONTRACTS" : "STORY";
    }
}
