/**
 * Server-side combat validation for F4 (player hit authority).
 * Health is normalized 0..1. Damage requests are clamped and rate-limited.
 */
public final class CombatAuthorityRules
{
    public static final float MAX_DAMAGE = 0.35f;
    public static final float MIN_DAMAGE = 0.01f;
    public static final float DEFAULT_HEALTH = 1.0f;
    public static final long HIT_COOLDOWN_NANOS = 200_000_000L; // 200ms
    public static final float MAX_HIT_DISTANCE = 8.0f;

    private CombatAuthorityRules()
    {
    }

    public static float clampDamage(float requested)
    {
        if (Float.isNaN(requested) || Float.isInfinite(requested))
        {
            return 0.0f;
        }

        if (requested < MIN_DAMAGE)
        {
            return 0.0f;
        }

        return Math.min(requested, MAX_DAMAGE);
    }

    public static boolean cooldownElapsed(long nowNanos, long lastHitNanos)
    {
        if (lastHitNanos <= 0L)
        {
            return true;
        }

        return (nowNanos - lastHitNanos) >= HIT_COOLDOWN_NANOS;
    }

    public static float applyDamage(float currentHealth, float damage)
    {
        float next = currentHealth - damage;
        if (next < 0.0f)
        {
            return 0.0f;
        }

        if (next > 1.0f)
        {
            return 1.0f;
        }

        return next;
    }

    public static boolean withinHitRange(
            boolean haveAttackerPos,
            boolean haveTargetPos,
            float ax,
            float ay,
            float az,
            float tx,
            float ty,
            float tz)
    {
        if (!haveAttackerPos || !haveTargetPos)
        {
            // No MOVE samples yet — allow but rely on damage clamp + cooldown.
            return true;
        }

        float dx = ax - tx;
        float dy = ay - ty;
        float dz = az - tz;
        float distSq = dx * dx + dy * dy + dz * dz;
        float max = MAX_HIT_DISTANCE;
        return distSq <= (max * max);
    }
}
