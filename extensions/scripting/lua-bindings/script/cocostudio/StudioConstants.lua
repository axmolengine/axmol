if nil == ccs then
    return
end

axext.MovementEventType = {
    start = 0,
    complete = 1,
    loopComplete = 2,
}

axext.InnerActionType = {
    LoopAction = 0,
    NoLoopAction = 1,
    SingleFrame = 2,
}
