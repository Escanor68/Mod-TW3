import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;

/**
 * Optional shared-secret check for UDP packets (R5).
 * When the configured token is empty, auth is not required (dev/LAN default).
 */
public final class PacketAuthRules
{
    private PacketAuthRules()
    {
    }

    public static boolean isRequired(String configuredToken)
    {
        return configuredToken != null && !configuredToken.trim().isEmpty();
    }

    public static boolean matches(String configuredToken, String providedToken)
    {
        if (!isRequired(configuredToken))
        {
            return true;
        }

        if (providedToken == null)
        {
            return false;
        }

        byte[] expected = configuredToken.trim().getBytes(StandardCharsets.UTF_8);
        byte[] actual = providedToken.trim().getBytes(StandardCharsets.UTF_8);
        return MessageDigest.isEqual(expected, actual);
    }
}
