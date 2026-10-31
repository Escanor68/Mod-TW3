/**
 * Per-party story/contracts room seat (F7). Keyed by {@link PartyScopeRules#partyKey}.
 */
public final class PartyRoomState
{
    public volatile RoomCharacterRules.RoomMode mode = RoomCharacterRules.RoomMode.STORY;
    /** Display username of Geralt owner in story mode; empty when none. */
    public volatile String ownerUsername = "";

    public boolean hasOwner()
    {
        return ownerUsername != null && !ownerUsername.trim().isEmpty();
    }
}
