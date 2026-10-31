// Witcher Online — party-scoped remote NPC presence (F3)
// Visual ghosts for NPCs published by party members.

class wo_RemoteNpcEntry
{
	public var compositeKey : string;
	public var npcKey : string;
	public var templatePath : string;
	public var publisher : string;
	public var entity : CActor;
	public var lastUpdate : float;
	public var pos : Vector;
	public var yaw : float;
	public var hp : float;
	public var stateName : string;
}
