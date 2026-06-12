if nil == ccs then
    return
end

ax.MovementEventType = {
    start = 0,
    complete = 1,
    loopComplete = 2,
}

ax.InnerActionType = {
    LoopAction = 0,
    NoLoopAction = 1,
    SingleFrame = 2,
}
