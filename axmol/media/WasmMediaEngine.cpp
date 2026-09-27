/****************************************************************************
 Copyright (c) 2026 Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#if defined(__EMSCRIPTEN__)
#    include "axmol/media/WasmMediaEngine.h"
#    include <emscripten/emscripten.h>

namespace ax
{

// Browser state belongs to each media engine instance. C++ reads only completed frames.
extern "C" EMSCRIPTEN_KEEPALIVE void wasm_media_fire_event(WasmMediaEngine* engine, int event)
{
    if (engine)
        engine->dispatchEvent(static_cast<MEMediaEventType>(event));
}

WasmMediaEngine::WasmMediaEngine()
{
    // clang-format off
    _id = EM_ASM_INT({
        const engine = $0;
        const media = Module.axmolWasmMedia || (Module.axmolWasmMedia = {nextId: 1, players: new Map()});
        const id = media.nextId++;
        media.players.set(id, {engine, video: null, url: null, generation: 0, state: 0,
                               frame: null, canvas: null, pending: false, callbackId: 0, ended: false,
                               wantedPlay: false, playPending: false, playToken: 0, frameToken: 0,
                               autoPlay: false, loop: false, rate: 1, volume: 1});
        return id;
    }, this);
    // clang-format on
}

WasmMediaEngine::~WasmMediaEngine()
{
    close();
    // clang-format off
    EM_ASM({
        const media = Module.axmolWasmMedia;
        const player = media && media.players.get($0);
        if (player) {
            player.engine = 0;
            media.players.delete($0);
        }
    }, _id);
    // clang-format on
}

void WasmMediaEngine::setCallbacks(std::function<void(MEMediaEventType)> onMediaEvent,
                                   std::function<void(const MEVideoFrame&)> onVideoFrame)
{
    _onMediaEvent = std::move(onMediaEvent);
    _onVideoFrame = std::move(onVideoFrame);
}

void WasmMediaEngine::setAutoPlay(bool autoPlay)
{
    _autoPlay = autoPlay;
}

bool WasmMediaEngine::open(std::string_view sourceUri)
{
    close();
    if (sourceUri.empty())
        return false;

    // clang-format off
    return EM_ASM_INT({
        const id = $0;
        const source = $1;
        const length = $2;
        const autoPlay = $3;
        const loop = $4;
        const rate = $5;
        const volume = $6;
        const r = Module.axmolWasmMedia.players.get(id);
        if (!r) return 0;
        const sourceUri = UTF8ToString(source, length);
        if (!sourceUri) return 0;
        const generation = r.generation;
        // Closing or replacing the source invalidates every callback that captured this generation.
        r.emit = (event) => {
            if (r.generation === generation && r.engine)
                Module._wasm_media_fire_event(r.engine, event);
        };
        let url;
        try {
            if (/^[a-z][a-z0-9+.-]*:/i.test(sourceUri) && !sourceUri.startsWith('file:')) {
                url = sourceUri;
            } else {
                const path = sourceUri.startsWith('file://') ? sourceUri.slice(7) : sourceUri;
                const bytes = FS.readFile(path);
                const extension = path.split('.').pop().toLowerCase();
                const mime = ({mp4: 'video/mp4', m4v: 'video/mp4', webm: 'video/webm',
                               ogv: 'video/ogg', mov: 'video/quicktime'})[extension] || 'application/octet-stream';
                url = URL.createObjectURL(new Blob([bytes], {type: mime}));
                r.url = url;
            }
            const video = document.createElement('video');
            video.preload = 'auto';
            video.playsInline = true;
            video.loop = !!loop;
            video.playbackRate = rate;
            video.volume = volume;
            if (/^https?:/i.test(url)) video.crossOrigin = 'anonymous';
            r.video = video;
            r.state = 1;
            r.autoPlay = !!autoPlay;
            r.wantedPlay = !!autoPlay;
            r.ended = false;
            const active = () => r.video === video && r.generation === generation;
            const fail = (error) => {
                if (!active()) return;
                r.playPending = false;
                // A rejected play() is recoverable after a user gesture.
                r.state = error && error.name === 'NotAllowedError' ? 3 : 5;
                r.wantedPlay = false;
                r.emit(3);
                console.warn('Axmol WasmMediaEngine:', error);
                if (r.callbackId && video.cancelVideoFrameCallback)
                    video.cancelVideoFrameCallback(r.callbackId);
                r.callbackId = 0;
                if (!video.paused) video.pause();
            };
            const tryPlay = () => {
                if (!active() || !r.wantedPlay || r.playPending || !video.paused) return;
                let promise;
                const playToken = ++r.playToken;
                r.playPending = true;
                try { promise = video.play(); } catch (error) { fail(error); return; }
                if (promise && promise.then)
                    promise.then(() => {
                        if (active() && r.playToken === playToken) r.playPending = false;
                    }, error => {
                        if (active() && r.playToken === playToken && r.wantedPlay) fail(error);
                    });
                else r.playPending = false;
            };
            const schedule = () => {
                if (!active() || r.state !== 2 || r.callbackId) return;
                if (!video.requestVideoFrameCallback) {
                    fail(new Error('requestVideoFrameCallback is unavailable'));
                    return;
                }
                r.callbackId = video.requestVideoFrameCallback(async (_now, metadata) => {
                    r.callbackId = 0;
                    if (!active() || r.state !== 2) return;
                    if (!r.pending && video.videoWidth && video.videoHeight) {
                        r.pending = true;
                        const frameToken = r.frameToken;
                        let frame;
                        try {
                            const width = video.videoWidth, height = video.videoHeight;
                            let format = 0, data, cbcrOffset = 0;
                            if (typeof VideoFrame !== 'undefined') {
                                frame = new VideoFrame(video, {timestamp: Math.round(metadata.mediaTime * 1000000)});
                                const color = frame.colorSpace;
                                const yuv = (width % 2 === 0 && height % 2 === 0 &&
                                    frame.codedWidth === width && frame.codedHeight === height &&
                                    frame.displayWidth === width && frame.displayHeight === height &&
                                    frame.visibleRect && frame.visibleRect.x === 0 && frame.visibleRect.y === 0 &&
                                    frame.visibleRect.width === width && frame.visibleRect.height === height &&
                                    color && color.matrix === 'bt709' && color.primaries === 'bt709' &&
                                    color.transfer === 'bt709' && color.fullRange === false);
                                if (yuv && (frame.format === 'I420' || frame.format === 'NV12')) {
                                    format = frame.format === 'I420' ? 5 : 4;
                                    cbcrOffset = width * height;
                                    const layout = format === 5
                                        ? [{offset: 0, stride: width},
                                           {offset: cbcrOffset, stride: width / 2},
                                           {offset: cbcrOffset + cbcrOffset / 4, stride: width / 2}]
                                        : [{offset: 0, stride: width}, {offset: cbcrOffset, stride: width}];
                                    data = new Uint8Array(cbcrOffset * 3 / 2);
                                    try { await frame.copyTo(data, {format: frame.format, layout}); }
                                    catch (_) { data = null; format = 0; }
                                }
                                if (!data && frame.codedWidth === width && frame.codedHeight === height &&
                                    frame.visibleRect && frame.visibleRect.x === 0 && frame.visibleRect.y === 0 &&
                                    frame.visibleRect.width === width && frame.visibleRect.height === height) {
                                    const rgba = new Uint8Array(width * height * 4);
                                    try {
                                        await frame.copyTo(rgba, {format: 'RGBA', layout: [{offset: 0, stride: width * 4}]});
                                        data = rgba;
                                    } catch (_) {}
                                }
                            }
                            if (!data) {
                                const canvas = r.canvas || (r.canvas = document.createElement('canvas'));
                                canvas.width = width;
                                canvas.height = height;
                                const context = canvas.getContext('2d', {willReadFrequently: true});
                                context.drawImage(video, 0, 0, width, height);
                                data = context.getImageData(0, 0, width, height).data;
                                format = 0;
                            }
                            if (active() && r.state === 2 && r.frameToken === frameToken)
                                r.frame = {data, width, height, format, cbcrOffset};
                        } catch (error) {
                            fail(error);
                        } finally {
                            if (frame) frame.close();
                            if (active()) r.pending = false;
                        }
                    }
                    if (active()) schedule();
                });
            };
            r.tryPlay = tryPlay;
            r.schedule = schedule;
            video.oncanplay = () => { if (active() && r.wantedPlay) tryPlay(); };
            video.onplaying = () => {
                if (!active()) return;
                r.state = 2;
                r.ended = false;
                r.emit(0);
                schedule();
            };
            video.onpause = () => {
                if (!active() || video.ended || r.state === 3 || r.state === 4 || r.state === 5) return;
                if (r.callbackId && video.cancelVideoFrameCallback)
                    video.cancelVideoFrameCallback(r.callbackId);
                r.callbackId = 0;
                r.frame = null;
                ++r.frameToken;
                r.state = 3;
                r.emit(1);
            };
            video.onended = () => {
                if (!active()) return;
                if (r.callbackId && video.cancelVideoFrameCallback)
                    video.cancelVideoFrameCallback(r.callbackId);
                r.callbackId = 0;
                r.state = 4;
                r.ended = true;
                r.wantedPlay = false;
                r.emit(2);
            };
            video.onerror = () => fail(video.error || new Error('Media loading failed'));
            video.src = url;
            video.load();
            if (autoPlay) tryPlay();
            return 1;
        } catch (error) {
            if (r.url) { URL.revokeObjectURL(r.url); r.url = null; }
            r.state = 5;
            r.emit(3);
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id, sourceUri.data(), static_cast<int>(sourceUri.size()), _autoPlay, _loop, _rate, _volume) != 0;
    // clang-format on
}

bool WasmMediaEngine::close()
{
    // clang-format off
    EM_ASM({
        const id = $0;
        const media = Module.axmolWasmMedia;
        const r = media && media.players.get(id);
        if (!r) return;
        ++r.generation;
        r.frame = null;
        r.canvas = null;
        r.pending = false;
        ++r.frameToken;
        r.playPending = false;
        ++r.playToken;
        r.wantedPlay = false;
        r.ended = false;
        r.state = 0;
        if (r.video) {
            const video = r.video;
            if (r.callbackId && video.cancelVideoFrameCallback)
                video.cancelVideoFrameCallback(r.callbackId);
            r.callbackId = 0;
            video.onplaying = video.onpause = video.onended = video.onerror = video.oncanplay = null;
            video.pause();
            video.removeAttribute('src');
            video.load();
            r.video = null;
        }
        if (r.url) {
            URL.revokeObjectURL(r.url);
            r.url = null;
        }
    }, _id);
    // clang-format on
    _frameBuffer.clear();
    return true;
}

bool WasmMediaEngine::setLoop(bool looping)
{
    // clang-format off
    if (!EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r) return 0;
        try {
            r.loop = !!$1;
            if (r.video) r.video.loop = r.loop;
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id, looping)) return false;
    // clang-format on
    _loop = looping;
    return true;
}

bool WasmMediaEngine::setRate(double rate)
{
    // clang-format off
    if (!EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r) return 0;
        try {
            if (r.video) r.video.playbackRate = $1;
            r.rate = $1;
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id, rate)) return false;
    // clang-format on
    _rate = rate;
    return true;
}

bool WasmMediaEngine::setVolume(double volume)
{
    // clang-format off
    if (!EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r) return 0;
        try {
            if (r.video) r.video.volume = $1;
            r.volume = $1;
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id, volume)) return false;
    // clang-format on
    _volume = volume;
    return true;
}

double WasmMediaEngine::getVolume() const
{
    return _volume;
}

bool WasmMediaEngine::setCurrentTime(double time)
{
    // clang-format off
    return EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r || !r.video) return 0;
        try {
            r.video.currentTime = $1;
            r.frame = null;
            ++r.frameToken;
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id, time) != 0;
    // clang-format on
}

double WasmMediaEngine::getCurrentTime()
{
    // clang-format off
    return EM_ASM_DOUBLE({
        const r = Module.axmolWasmMedia.players.get($0);
        return r && r.video ? r.video.currentTime : 0;
    }, _id);
    // clang-format on
}

double WasmMediaEngine::getDuration()
{
    // clang-format off
    return EM_ASM_DOUBLE({
        const r = Module.axmolWasmMedia.players.get($0);
        return r && r.video ? r.video.duration : 0;
    }, _id);
    // clang-format on
}

bool WasmMediaEngine::play()
{
    // clang-format off
    return EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r || !r.video) return 0;
        try {
            r.wantedPlay = true;
            r.ended = false;
            if (r.state === 4) r.video.currentTime = 0;
            r.tryPlay();
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id) != 0;
    // clang-format on
}

bool WasmMediaEngine::pause()
{
    // clang-format off
    return EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r || !r.video) return 0;
        try {
            r.wantedPlay = false;
            r.playPending = false;
            ++r.playToken;
            r.video.pause();
            if (r.state === 1) {
                r.state = 3;
                r.emit(1);
            }
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id) != 0;
    // clang-format on
}

bool WasmMediaEngine::stop()
{
    // clang-format off
    return EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r || !r.video) return 0;
        try {
            r.wantedPlay = false;
            r.playPending = false;
            ++r.playToken;
            r.state = 4;
            r.ended = false;
            r.frame = null;
            ++r.frameToken;
            if (r.callbackId && r.video.cancelVideoFrameCallback)
                r.video.cancelVideoFrameCallback(r.callbackId);
            r.callbackId = 0;
            r.video.pause();
            r.video.currentTime = 0;
            r.emit(2);
            return 1;
        } catch (error) {
            console.warn('Axmol WasmMediaEngine:', error);
            return 0;
        }
    }, _id) != 0;
    // clang-format on
}

bool WasmMediaEngine::isPlaybackEnded() const
{
    // clang-format off
    return EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        return r && r.ended ? 1 : 0;
    }, _id) != 0;
    // clang-format on
}

void WasmMediaEngine::dispatchEvent(MEMediaEventType event)
{
    if (_onMediaEvent)
        _onMediaEvent(event);
}

MEMediaState WasmMediaEngine::getState() const
{
    // clang-format off
    return static_cast<MEMediaState>(EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        return r ? r.state : 0;
    }, _id));
    // clang-format on
}

bool WasmMediaEngine::transferVideoFrame()
{
    int info[5];
    if (!_onVideoFrame)
        return false;
    // clang-format off
    if (!EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r || !r.frame) return 0;
        const f = r.frame;
        const p = $1 >> 2;
        HEAP32[p] = f.width;
        HEAP32[p + 1] = f.height;
        HEAP32[p + 2] = f.format;
        HEAP32[p + 3] = f.cbcrOffset;
        HEAP32[p + 4] = f.data.length;
        return 1;
    }, _id, info))
        return false;
    // clang-format on
    if (info[0] <= 0 || info[1] <= 0 || info[4] <= 0)
        return false;
    _frameBuffer.resize(static_cast<size_t>(info[4]));
    // clang-format off
    if (!EM_ASM_INT({
        const r = Module.axmolWasmMedia.players.get($0);
        if (!r || !r.frame || r.frame.data.length > $2) return 0;
        HEAPU8.set(r.frame.data, $1);
        r.frame = null;
        return 1;
    }, _id, _frameBuffer.data(), info[4]))
        return false;
    // clang-format on
    const auto format = static_cast<MEVideoPixelFormat>(info[2]);
    const MEIntPoint dimensions(info[0], info[1]);
    MEVideoPixelDesc desc(format, dimensions);
    // VideoPlayer currently selects its BT.709 limited-range matrix when this flag is true.
    // Keep its rendering path unchanged; only limited-range YUV is emitted above.
    desc._fullRange       = true;
    const uint8_t* chroma = info[3] ? _frameBuffer.data() + info[3] : nullptr;
    _onVideoFrame(MEVideoFrame(_frameBuffer.data(), chroma, _frameBuffer.size(), desc, dimensions));
    return true;
}

}  // namespace ax

#endif
