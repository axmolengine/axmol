
function schedule(node, callback, delay)
    local delay = ax.DelayTime:create(delay)
    local sequence = ax.Sequence:create(delay, ax.CallFunc:create(callback))
    local action = ax.RepeatForever:create(sequence)
    node:runAction(action)
    return action
end

function performWithDelay(node, callback, delay)
    local delay = ax.DelayTime:create(delay)
    local sequence = ax.Sequence:create(delay, ax.CallFunc:create(callback))
    node:runAction(sequence)
    return sequence
end
