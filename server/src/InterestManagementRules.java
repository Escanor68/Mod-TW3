/**
 * Proximity interest management for F6.
 * Party members always share interest; otherwise relay only within radius.
 * Missing MOVE samples fail open (allow) until both sides have positions.
 */
public final class InterestManagementRules
{
    public static final float DEFAULT_INTEREST_RADIUS = 100.0f;

    private InterestManagementRules()
    {
    }

    public static boolean withinInterest(
            boolean publisherHasPos,
            boolean recipientHasPos,
            float px,
            float py,
            float pz,
            float rx,
            float ry,
            float rz,
            float radius)
    {
        if (!publisherHasPos || !recipientHasPos)
        {
            return true;
        }

        float max = radius;
        if (max <= 0.0f || Float.isNaN(max) || Float.isInfinite(max))
        {
            max = DEFAULT_INTEREST_RADIUS;
        }

        float dx = px - rx;
        float dy = py - ry;
        float dz = pz - rz;
        float distSq = dx * dx + dy * dy + dz * dz;
        return distSq <= (max * max);
    }

    /**
     * @param sameParty when true, always relay (F2/F3 party cohesion)
     */
    public static boolean shouldRelay(
            boolean sameParty,
            boolean publisherHasPos,
            boolean recipientHasPos,
            float px,
            float py,
            float pz,
            float rx,
            float ry,
            float rz,
            float radius)
    {
        if (sameParty)
        {
            return true;
        }

        return withinInterest(
                publisherHasPos,
                recipientHasPos,
                px,
                py,
                pz,
                rx,
                ry,
                rz,
                radius);
    }

    /**
     * Upper bound on pairwise broadcasts when all players are mutually out of range
     * and not in party: each publisher reaches only itself (1), not N.
     */
    public static int maxRecipientsWhenIsolated(int playerCount)
    {
        if (playerCount <= 0)
        {
            return 0;
        }

        return 1;
    }

    public static int baselineGlobalRecipients(int playerCount)
    {
        return Math.max(0, playerCount);
    }
}
