/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
package dev.axmol.lib;

import android.content.Context;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.net.Uri;
import android.os.Handler;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.media3.common.Format;
import androidx.media3.common.MediaItem;
import androidx.media3.common.PlaybackException;
import androidx.media3.common.Player;
import androidx.media3.common.VideoSize;
import androidx.media3.common.util.UnstableApi;
import androidx.media3.datasource.DataSource;
import androidx.media3.datasource.DefaultDataSource;
import androidx.media3.exoplayer.DefaultRenderersFactory;
import androidx.media3.exoplayer.ExoPlayer;
import androidx.media3.exoplayer.Renderer;
import androidx.media3.exoplayer.mediacodec.MediaCodecAdapter;
import androidx.media3.exoplayer.mediacodec.MediaCodecSelector;
import androidx.media3.exoplayer.source.MediaSource;
import androidx.media3.exoplayer.source.ProgressiveMediaSource;
import androidx.media3.exoplayer.video.VideoFrameMetadataListener;
import androidx.media3.exoplayer.video.VideoRendererEventListener;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

@UnstableApi @SuppressWarnings("unused")
public class AxmolMediaPlayer extends DefaultRenderersFactory implements Player.Listener, ByteBufferVideoRenderer.VideoFrameProcessor, VideoFrameMetadataListener {
    // The native media events, match with MEMediaEventType
    public static final int EVENT_PLAYING = 0;
    public static final int EVENT_PAUSED = 1;
    public static final int EVENT_STOPPED = 2;
    public static final int EVENT_ERROR = 3;

    // The native video pixel formats, match with MEVideoPixelFormat
    public static final int VIDEO_PF_NV12 = 3;
    public static final int VIDEO_PF_I420 = 4;

    /**
     * Media has been closed and cannot be played again.
     */
    public static final int STATE_CLOSED = 0;

    /**
     * Media is being prepared for playback.
     */
    public static final int STATE_PREPARING = 1;

    /**
     * Media is currently playing.
     */
    public static final int STATE_PLAYING = 2;

    /**
     * Playback has been paused, but can be resumed.
     */
    public static final int STATE_PAUSED = 3;

    /**
     * Playback has been stopped, but can be restarted.
     */
    public static final int STATE_STOPPED = 4;

    /**
     * Unrecoverable error occurred during playback.
     */
    public static final int STATE_ERROR = 5;

    // The desired video pixel format
    /**
     * COLOR_FormatYUV420SemiPlanar (yyyyyyyy uvuv)  (NV12)
     * COLOR_FormatYUV420Planar (yyyyyyyy uu vv)     (YUV420p)
     * COLOR_FormatYUV422SemiPlanar (Y0 U0 Y1 V0)    (YUY2)
     */
    public static final String TAG = "AxmolMediaPlayer";
    private Context mContext;
    private ExoPlayer mPlayer;
    private volatile ByteBufferVideoRenderer mVideoRenderer;
    private MediaFormat mOutputFormat;
    private volatile long mNativeObj = 0; // native object address for send event to C++, weak ref
    private boolean mAutoPlay = false;
    private boolean mLooping = false;
    private volatile boolean mPlayWhenReady = false;
    private volatile boolean mPlaybackEnded = false;
    private AtomicInteger mState = new AtomicInteger(STATE_CLOSED);

    /**
     * ------ native methods -------
     */
    public static native void nativeFireEvent(long nativeObj, int arg1);

    public static native void nativeSetDuration(long nativeObj, double duration);
    public static native void nativeSetVideoMeta(long mNativeObj, int outputX, int outputY, int videoX, int videoY, int cbcrOffset, int rotation, int videoPF);

    public static native void nativeProcessVideoFrame(long nativeObj, ByteBuffer sampleData, int sampleLen, long presentationTimeUs);

    @SuppressWarnings("unused")
    public static Object createMediaPlayer(long nativeObj) {
        return new AxmolMediaPlayer(AxmolEngine.getApplicationContext(), nativeObj);
    }

    /**
     * @param context A {@link Context}.
     */
    public AxmolMediaPlayer(Context context, long nativeObj) {
        super(context);

        mContext = context;
        mNativeObj = nativeObj;
        setAllowedVideoJoiningTimeMs(0);
    }

    @SuppressWarnings("unused")
    public void dispose() {
        mNativeObj = 0;
        close();
    }

    @Override
    protected void buildVideoRenderers(
        @NonNull Context context,
        @DefaultRenderersFactory.ExtensionRendererMode int extensionRendererMode,
        @NonNull MediaCodecSelector mediaCodecSelector,
        boolean enableDecoderFallback,
        @NonNull Handler eventHandler,
        @NonNull VideoRendererEventListener eventListener,
        long allowedVideoJoiningTimeMs,
        ArrayList<Renderer> out) {
        out.add(
            new ByteBufferVideoRenderer(
                context,
                getCodecAdapterFactory(),
                mediaCodecSelector,
                allowedVideoJoiningTimeMs,
                enableDecoderFallback,
                eventHandler,
                eventListener,
                MAX_DROPPED_VIDEO_FRAME_COUNT_TO_NOTIFY));
    }

    public void setAutoPlay(boolean bAutoPlay) {
        mAutoPlay = bAutoPlay;
        mPlayWhenReady = bAutoPlay;
    }

    /**
     * @param sourceUri: url: http(s)://
     *                   disk: file://path/to/file
     *                   assets: file:///android_asset
     * @return
     */
    public boolean open(String sourceUri) {
        if (mState.get() == STATE_PREPARING)
            return false;
        mState.set(STATE_PREPARING);

        final AxmolMediaPlayer mediaPlayer = this;
        AxmolEngine.runOnUiThread(() -> {
            try {
                DataSource.Factory dataSourceFactory = new DefaultDataSource.Factory(mContext);
                MediaSource mediaSource =
                    new ProgressiveMediaSource.Factory(dataSourceFactory)
                        .createMediaSource(MediaItem.fromUri(Uri.parse(sourceUri)));

                mPlayer = new ExoPlayer.Builder(mContext, mediaPlayer).build();
                for (int i = 0; i < mPlayer.getRendererCount(); i++) {
                    Renderer renderer = mPlayer.getRenderer(i);
                    if (renderer instanceof ByteBufferVideoRenderer) {
                        mVideoRenderer = (ByteBufferVideoRenderer) renderer;
                        break;
                    }
                }
                if (mVideoRenderer != null) {
                    mVideoRenderer.setOutput(mediaPlayer);
                }
                mPlayer.setVideoFrameMetadataListener(mediaPlayer);
                mPlayer.addListener(mediaPlayer);
                mPlayer.setMediaSource(mediaSource);
                mPlaybackEnded = false;
                mPlayer.setRepeatMode(mLooping ? Player.REPEAT_MODE_ALL : Player.REPEAT_MODE_OFF);
                mPlayer.prepare();
                mPlayWhenReady = mAutoPlay;
                mPlayer.setPlayWhenReady(mAutoPlay);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        });

        return true;
    }

    public boolean close() {
        if (mPlayer != null) {
            final ExoPlayer player = mPlayer;
            mPlayer = null;
            mPlayWhenReady = false;
            final AxmolMediaPlayer mediaPlayer = this;
            AxmolEngine.runOnUiThread(() -> {
                if (mVideoRenderer != null) {
                    mVideoRenderer.setOutput(null);
                }
                player.removeListener(mediaPlayer);
                player.stop();
                player.release();
                mVideoRenderer = null;
                mState.set(STATE_CLOSED);
            });
        }
        return true;
    }

    public boolean setLoop(boolean bLooping) {
        if (mLooping != bLooping) {
            mLooping = bLooping;
            if (mPlayer == null) return false;
            AxmolEngine.runOnUiThread(() -> {
                if (mPlayer != null)
                    mPlayer.setRepeatMode(mLooping ? Player.REPEAT_MODE_ALL : Player.REPEAT_MODE_OFF);
            });
        }
        return true;
    }

    @SuppressWarnings("unused")
    public boolean setRate(double fRate) {
        if (mPlayer == null) return false;
        AxmolEngine.runOnUiThread(() -> {
            if (mPlayer != null)
                mPlayer.setPlaybackSpeed((float) fRate);
        });
        return true;
    }

    public boolean setCurrentTime(double fSeekTimeInSec) {
        if (mPlayer == null) return false;

        AxmolEngine.runOnUiThread(() -> {
            if (mPlayer != null) {
                mPlaybackEnded = false;
                mPlayer.seekTo((long) (fSeekTimeInSec * 1000));
            }
        });
        return true;
    }

    public boolean play() {
        if (mPlayer == null) return false;
        AxmolEngine.runOnUiThread(() -> {
            if (mPlayer != null) {
                mPlayWhenReady = true;
                mPlaybackEnded = false;

                int playbackState = mPlayer.getPlaybackState();
                boolean restartingFromStopped = mState.compareAndSet(STATE_STOPPED, STATE_PREPARING);
                if (playbackState == Player.STATE_IDLE) {
                    mPlayer.prepare();
                }
                if (restartingFromStopped || playbackState == Player.STATE_ENDED) {
                    mPlayer.seekTo(0);
                }
                mPlayer.play();
            }
        });
        return true;
    }

    public boolean pause() {
        if (mPlayer == null) return false;
        AxmolEngine.runOnUiThread(() -> {
            if (mPlayer != null) {
                mPlayWhenReady = false;
                mPlayer.pause();
            }
        });
        return true;
    }

    public boolean stop() {
        if (mPlayer == null) return false;
        AxmolEngine.runOnUiThread(() -> {
            if (mPlayer != null) {
                mPlayWhenReady = false;
                mPlayer.stop();
            }
        });
        return true;
    }

    /**
     * Get playback State match with native MEMediaState
     */
    public int getState() {
        return mState.get();
    }

    @Override
    public void onVideoFrameAboutToBeRendered(
        long presentationTimeUs,
        long releaseTimeNs,
        Format format,
        @Nullable MediaFormat mediaFormat) {
        if (mOutputFormat != mediaFormat) {
            // format.sampleMimeType will be video/hevc or video/avc
            mOutputFormat = mediaFormat;
            handleVideoMetaChanged();
        }
    }

    /* handle video informations changed
     * Notes
     * 1. About desired frame size bytes
     *   a. stride > mOutputDim.x: means all frame bytes should pass to GPU(shader), and
     *      desired frame size bytes is: stride * sliceHeight * 3 / 2
     *   b. stride == mOutputDim.x: means we need discard Y plane aligned extra data, and
     *      desired frame size bytes is: stride * sliceHeight + (mOutputDim.x / 2) * (mOutputDim.y / 2) * 2
     * 2. About video frame size alignment
     *   a. many devices may align 2, the sliceHeight == mOutputDim.y and stride == mOutputDim.x
     *   b. H264: align 16 for both width and height
     *      HEVC/H265: align 32 for both width and height
     * 3. The cbcrOffset should be always stride * sliceHeight
     *   refer: https://github.com/axmolengine/axmol/issues/2101
     */
    private void handleVideoMetaChanged() {
        MediaFormat format = mOutputFormat;
        if (format != null) {
            // String mimeType = format.getString(MediaFormat.KEY_MIME); // =="video/raw"
            // Note: some android 11 and older devices not response desired color format(NV12), instead will be YUV420P aka I420
            // refer: https://github.com/axmolengine/axmol/issues/2049
            int videoPF;
            Integer colorFormat = format.getInteger(MediaFormat.KEY_COLOR_FORMAT);
            switch (colorFormat) {
                case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible:
                case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
                    videoPF = VIDEO_PF_NV12;
                    break;
                case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
                    videoPF = VIDEO_PF_I420;
                    break;
                default:
                    videoPF = VIDEO_PF_NV12;
                    Log.w(TAG, String.format("Unsupported color format: %d, video render may incorrect!", colorFormat));
            }

            // output dim
            int outputX = format.getInteger(MediaFormat.KEY_WIDTH);
            int outputY = format.getInteger(MediaFormat.KEY_HEIGHT);

            int stride = 0;
            int sliceHeight = 0;
            if (format.containsKey(MediaFormat.KEY_STRIDE)) {
                stride = format.getInteger(MediaFormat.KEY_STRIDE);
            }
            if (format.containsKey(MediaFormat.KEY_SLICE_HEIGHT)) {
                sliceHeight = format.getInteger(MediaFormat.KEY_SLICE_HEIGHT);
            }
            Log.d(TAG, String.format("Frame stride and slice height: %dx%d", stride, sliceHeight));
            stride = Math.max(outputX, stride);
            sliceHeight = Math.max(outputY, sliceHeight);

            int cbcrOffset = stride * sliceHeight;
            int frameSizeBytes = 0;
            if (stride > outputX) {
                outputX = stride;
                outputY = sliceHeight;
                frameSizeBytes = cbcrOffset * 3 / 2;
            } else {
                frameSizeBytes = cbcrOffset + outputX / 2 * outputY;
            }

            // video dim
            int videoX = format.containsKey(MediaFormat.KEY_CROP_LEFT)
                && format.containsKey(MediaFormat.KEY_CROP_RIGHT) ? format.getInteger(MediaFormat.KEY_CROP_RIGHT) + 1
                - format.getInteger(MediaFormat.KEY_CROP_LEFT) : outputX;
            int videoY = format.containsKey(MediaFormat.KEY_CROP_TOP)
                && format.containsKey(MediaFormat.KEY_CROP_BOTTOM) ? format.getInteger(MediaFormat.KEY_CROP_BOTTOM) + 1
                - format.getInteger(MediaFormat.KEY_CROP_TOP) : outputY;

            // video rotation
            int rotation = format.containsKey(MediaFormat.KEY_ROTATION) ? format.getInteger(MediaFormat.KEY_ROTATION) : 0;

            nativeSetVideoMeta(mNativeObj, outputX, outputY, videoX, videoY, cbcrOffset, rotation, videoPF);

            Log.d(TAG, String.format("Input format:%s, outputDim:%dx%d, videoDim:%dx%d, cbcrOffset:%d, frameSizeBytes:%d",
                mVideoRenderer != null ? mVideoRenderer.getCodecName() : "unknown",
                outputX, outputY,
                videoX, videoY,
                cbcrOffset, frameSizeBytes));
        }
    }

    /**
     * handler or listener methods
     */
    @Override
    public void processVideoFrame(MediaCodecAdapter codec, int index, long presentationTimeUs) {
        if (mPlayWhenReady && mState.get() != STATE_PLAYING) {
            mPlaybackEnded = false;
            mState.set(STATE_PLAYING);
            nativeEvent(EVENT_PLAYING);
        }

        if (mNativeObj == 0) {
            return;
        }

        ByteBuffer tmpBuffer = codec.getOutputBuffer(index);
        if (tmpBuffer == null) {
            return;
        }
        nativeProcessVideoFrame(mNativeObj, tmpBuffer, tmpBuffer.remaining(), presentationTimeUs);
    }

    @Override
    public void onIsPlayingChanged(boolean isPlaying) {
        Log.d(TAG, "[Individual]onIsPlayingChanged: " + isPlaying);
        if (mPlayer == null) return;
        if (!isPlaying) {
            AxmolEngine.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (mPlayer == null) return;
                    int playbackState = mPlayer.getPlaybackState();
                    if (!mPlayer.getPlayWhenReady()
                        && (playbackState == Player.STATE_READY || playbackState == Player.STATE_BUFFERING)) {
                        mPlayWhenReady = false;
                        mState.set(STATE_PAUSED);
                        nativeEvent(EVENT_PAUSED);
                    } else if (playbackState == Player.STATE_IDLE && mState.get() != STATE_STOPPED) {
                        if (isLoopPlaybackActive()) return;
                        mState.set(STATE_STOPPED);
                        nativeEvent(EVENT_STOPPED);
                    }
                }
            });
        }
    }

    /**
     * @param playbackState int STATE_IDLE = 1;
     *                      int STATE_BUFFERING = 2;
     *                      int STATE_READY = 3;
     *                      int STATE_ENDED = 4;
     */
    @Override
    public void onPlaybackStateChanged(int playbackState) {
        Log.d(TAG, "[Individual]onPlaybackStateChanged: " + playbackState);
        if (mPlayer == null) return;
        switch (playbackState) {
            case Player.STATE_READY:
                AxmolEngine.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayer == null) return;
                        nativeSetDuration(mNativeObj, mPlayer.getContentDuration() / 1000.0);
                    }
                });
                break;
            case Player.STATE_ENDED:
                if (!isLoopPlaybackActive()) {
                    mPlaybackEnded = true;
                    mPlayWhenReady = false;
                    mState.set(STATE_STOPPED);
                    nativeEvent(EVENT_STOPPED);
                }
                break;
            case Player.STATE_IDLE:
                if (!isLoopPlaybackActive()) {
                    mState.set(STATE_STOPPED);
                    nativeEvent(EVENT_STOPPED);
                }
                break;
            default:
                ;
        }
    }

    @Override
    public void onPlayerError(PlaybackException error) {
        Log.e(TAG, "onPlayerError: " + error.getMessage());
        if (mPlayer == null) return;
        mState.set(STATE_ERROR);
        nativeEvent(EVENT_ERROR);
    }

    public boolean isPlaybackEnded() {
        return mPlaybackEnded;
    }

    @Override
    public void onVideoSizeChanged(VideoSize videoSize) {
        Log.d(TAG, String.format("[Individual]onVideoSizeChanged: (%d,%d)", videoSize.width, videoSize.height));

        if (mPlayer != null)
            handleVideoMetaChanged();
    }

    @Override
    public void onIsLoadingChanged(boolean isLoading) {
        Log.d(TAG, "[Individual]onIsLoadingChanged: " + isLoading);
    }

    public void nativeEvent(int event) {
        if (mNativeObj != 0 && mPlayer != null) {
            nativeFireEvent(mNativeObj, event);
        }
    }

    private boolean isLoopPlaybackActive() {
        return mLooping && mPlayWhenReady && mPlayer != null;
    }
}
