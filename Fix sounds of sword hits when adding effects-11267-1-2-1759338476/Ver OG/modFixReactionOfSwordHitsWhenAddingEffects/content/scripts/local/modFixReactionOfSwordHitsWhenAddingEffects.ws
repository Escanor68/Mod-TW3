function AddHitReactionForCriticalEffects(action : W3DamageAction)
{
	var reaction : W3DamageAction;
	
	if(!action.DealsAnyDamage()
	|| (action.victim == thePlayer && !action.IsActionRanged())
	|| ((CActor)action.victim).HasBuff(EET_HeavyKnockdown)
	|| ((CActor)action.victim).HasBuff(EET_Knockdown)
	|| ((CActor)action.victim).HasBuff(EET_LongStagger)
	|| ((CActor)action.victim).HasBuff(EET_Stagger)
	) return;
	
	((CActor)action.victim).SetCanPlayHitAnim( true );
	reaction = new W3DamageAction in theGame;
	reaction.Initialize( action.attacker, action.victim, NULL, "", EHRT_Light, CPS_Undefined, false, false, false, false );
	reaction.SetHitAnimationPlayType(EAHA_ForceYes);
	reaction.SetCannotReturnDamage(true);
	reaction.SetCanPlayHitParticle(false);
	reaction.SetCriticalHit();
	theGame.damageMgr.ProcessAction( reaction );
	delete reaction;
}