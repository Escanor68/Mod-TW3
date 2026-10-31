import java.util.Locale;

/**
 * Party membership helpers for scoped relays (F3 NPC presence).
 * Leader key: follower's joinedParty, otherwise the player's own username.
 */
public final class PartyScopeRules
{
    private PartyScopeRules()
    {
    }

    public static String normalizeName(String name)
    {
        return name == null ? "" : name.trim().toLowerCase(Locale.ROOT);
    }

    public static String partyKey(boolean inParty, String joinedParty, String username)
    {
        String joined = normalizeName(joinedParty);
        if (inParty && !joined.isEmpty() && !joined.equals("none"))
        {
            return joined;
        }

        return normalizeName(username);
    }

    public static boolean sameParty(
            boolean aInParty,
            String aJoinedParty,
            String aUsername,
            boolean bInParty,
            String bJoinedParty,
            String bUsername)
    {
        String aKey = partyKey(aInParty, aJoinedParty, aUsername);
        String bKey = partyKey(bInParty, bJoinedParty, bUsername);
        return !aKey.isEmpty() && aKey.equals(bKey);
    }

    public static boolean parseTruthy(String value)
    {
        if (value == null)
        {
            return false;
        }

        String lower = value.trim().toLowerCase(Locale.ROOT);
        return lower.equals("true") || lower.equals("1") || lower.equals("yes") || lower.equals("on");
    }
}
