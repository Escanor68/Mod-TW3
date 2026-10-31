import java.util.Locale;

/**
 * Pure session-identity helpers (username keys + reservation hold).
 * Kept free of I/O so operator rules stay unit-testable.
 */
public final class SessionIdentityRules
{
    private SessionIdentityRules()
    {
    }

    public static String normalizeUsernameKey(String username)
    {
        return username == null ? "" : username.trim().toLowerCase(Locale.ROOT);
    }

    /**
     * @return true if a timed-out username hold still blocks other IPs
     */
    public static boolean isReservationActive(long nowNanos, long expiresAtNanos)
    {
        return nowNanos < expiresAtNanos;
    }

    /**
     * Same IP may reclaim a reserved name; other IPs are blocked while active.
     */
    public static boolean canClaimReservedUsername(String reservationIp, String senderIp, long nowNanos, long expiresAtNanos)
    {
        if (!isReservationActive(nowNanos, expiresAtNanos))
        {
            return true;
        }

        if (reservationIp == null || senderIp == null)
        {
            return false;
        }

        return reservationIp.equals(senderIp);
    }
}
