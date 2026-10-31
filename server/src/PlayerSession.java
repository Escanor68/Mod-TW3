import java.util.Collections;
import java.util.List;

public class PlayerSession
{
    public final int playerId;
    public final String username;
    public volatile ClientEndpoint endpoint;
    public volatile long lastSeen;

    public volatile List<String> update1AFields = Collections.emptyList();
    public volatile List<String> update1BFields = Collections.emptyList();
    public volatile List<String> update2AFields = Collections.emptyList();
    public volatile List<String> update2BFields = Collections.emptyList();
    public volatile List<String> update3Fields = Collections.emptyList();
    public volatile List<String> update4Fields = Collections.emptyList();

    /** Parsed from UPDATE4 field 0/1 for party-scoped relays (F3). */
    public volatile boolean inParty = false;
    public volatile String joinedParty = "";

    /** Last MOVE sample for combat range checks (F4). */
    public volatile boolean hasPosition = false;
    public volatile float posX = 0.0f;
    public volatile float posY = 0.0f;
    public volatile float posZ = 0.0f;

    /** Authoritative normalized health 0..1 (F4). */
    public volatile float authHealth = CombatAuthorityRules.DEFAULT_HEALTH;
    public volatile long lastAuthHitNanos = 0L;

    public PlayerSession(int playerId, String username, ClientEndpoint endpoint, long lastSeen)
    {
        this.playerId = playerId;
        this.username = username;
        this.endpoint = endpoint;
        this.lastSeen = lastSeen;
    }
}