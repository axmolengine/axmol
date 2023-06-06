package org.axmol.lib;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Point;
import android.media.MediaCrypto;
import android.media.MediaFormat;
import android.net.Uri;
import android.os.Handler;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;

import com.google.android.exoplayer2.DefaultRenderersFactory;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.Format;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.Renderer;
import com.google.android.exoplayer2.mediacodec.MediaCodecAdapter;
import com.google.android.exoplayer2.mediacodec.MediaCodecInfo;
import com.google.android.exoplayer2.mediacodec.MediaCodecSelector;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.ProgressiveMediaSource;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultDataSource;
import com.google.android.exoplayer2.video.VideoRendererEventListener;
import com.google.android.exoplayer2.video.VideoSize;

import java.nio.ByteBuffer;
import java.util.ArrayList;

import android.media.MediaCodecInfo.CodecCapabilities;

public class AxmolMediaEngine implements Player.Listener, AxmolVideoRenderer.OutputHandler {
    // The native media events, match with MEMediaEventType
    public static final int EVENT_PLAYING = 0;
    public static final int EVENT_PAUSED = 1;
    public static final int EVENT_STOPPED = 2;
    public static final int EVENT_COMPLETED = 3;
    public static final int EVENT_ERROR = 4;

    /** Media has been closed and cannot be played again. */
    public static final int STATE_CLOSED = 0;

    public static final int STATE_CLOSING = 1;

    /** Unrecoverable error occurred during playback. */
    public static final int STATE_ERROR = 2;

    /** Playback has been paused, but can be resumed. */
    public static final int STATE_PAUSED = 3;

    /** Media is currently playing. */
    public static final int STATE_PLAYING = 4;

    /** Media is being prepared for playback. */
    public static final int STATE_PREPARING = 5;

    /** Playback has been stopped, but can be restarted. */
    public static final int STATE_STOPPED = 6;

    /** Playback has been completed, but can be restarted. */
    public static final int STATE_COMPLETED = 7;


    // The desired video pixel format
    /**
     * COLOR_FormatYUV420SemiPlanar (yyyyyyyy uvuv)  (NV12)
     * COLOR_FormatYUV420Planar (yyyyyyyy uu vv)     (YUV420p)
     * COLOR_FormatYUV422SemiPlanar (Y0 U0 Y1 V0)    (YUY2)
     */

    public static final String TAG = "AxmolMediaEngine";

    private ExoPlayer mPlayer;
    private AxmolVideoRenderer mVideoRenderer;
    private boolean mAutoPlay = false;
    private boolean mLooping = false;
    private long mNativeObj = 0; // native object address for send event to C++, weak ref

    private int mState = STATE_CLOSED;

    Point mOutputDim = new Point(); // The output dim match with buffer
    Point mVideoDim = new Point(); // The video dim (validate image dim)

    /** ------ native methods ------- */
    public static native void nativeHandleEvent(long nativeObj, int arg1);
    public static native void nativeHandleVideoSample(long nativeObj, ByteBuffer sampleData, int sampleLen, int outputX, int outputY, int videoX, int videoY);

    @SuppressWarnings("unused")
    public static Object createMediaEngine() {
        return new AxmolMediaEngine();
    }

    @SuppressWarnings("unused")
    public void bindNativeObject(long nativeObj) {
        mNativeObj = nativeObj;
    }

    public void setAutoPlay(boolean bAutoPlay)
    {
        mAutoPlay = bAutoPlay;
    }

    /**
     *
     * @param sourceUri:
     *                 url: http(s)://
     *                 disk: file://path/to/file
     *                 assets: file:///android_asset
     * @return
     */
    public boolean open(String sourceUri)
    {
        if(mState == STATE_PREPARING)
            return false;

        mState = STATE_PREPARING;
        final AxmolMediaEngine outputHandler = this;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    final Context activity = AxmolEngine.getActivity();
                    Context context = activity.getApplicationContext();
                    DataSource.Factory dataSourceFactory = new DefaultDataSource.Factory(activity);
                    MediaSource mediaSource =
                        new ProgressiveMediaSource.Factory(dataSourceFactory)
                            .createMediaSource(MediaItem.fromUri(Uri.parse(sourceUri)));

                    mPlayer = new ExoPlayer.Builder(context, new AxmolRenderersFactory(context)).build();
                    mVideoRenderer = (AxmolVideoRenderer) mPlayer.getRenderer(0); // the first must be video renderer
                    mVideoRenderer.setOutputHandler(outputHandler);
                    mPlayer.addListener(outputHandler);
                    mPlayer.setMediaSource(mediaSource);
                    mPlayer.prepare();
                    mPlayer.setRepeatMode(mLooping ? Player.REPEAT_MODE_ALL : Player.REPEAT_MODE_OFF);
                    mPlayer.setPlayWhenReady(mAutoPlay);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        });

        return true;
    }

    public boolean close() {
        if(mPlayer != null) {
            mState = STATE_CLOSING;
            final ExoPlayer player = mPlayer;
            mPlayer = null;
            final AxmolMediaEngine thiz = this;
            AxmolEngine.getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    player.removeListener(thiz);
                    player.stop();
                    player.release();
                    mState = STATE_CLOSED;
                }
            });
        }
        return true;
    }
    public boolean setLoop(boolean bLooping) {
        if (mLooping != bLooping) {
            mLooping = bLooping;
            if (mPlayer == null) return false;
            AxmolEngine.getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (mPlayer != null)
                        mPlayer.setRepeatMode(mLooping ? Player.REPEAT_MODE_ALL : Player.REPEAT_MODE_OFF);
                }
            });
        }
        return true;
    }

    @SuppressWarnings("unused")
    public boolean setRate(double fRate) {
        if(mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.setPlaybackSpeed((float)fRate);
            }
        });
        return true;
    }
    public boolean setCurrentTime(double fSeekTimeInSec)
    {
        if(mPlayer == null) return false;

        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.seekTo((long)(fSeekTimeInSec * 1000));
            }
        });
        return true;
    }
    public boolean play() {
        if (mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null) {
                    if (mState == STATE_STOPPED) // TO-CHECK: can't reply after playback stopped
                    {
                        /**
                         * The player is used in a way that may benefit from foreground mode.
                         * For this to be true, the same player instance must be used to play multiple pieces of content,
                         * and there must be gaps between the playbacks (i.e. Player.stop() is called to halt one playback,
                         * and prepare(com.google.android.exoplayer2.source.MediaSource) is called some time later to start a new one).
                         */
                        mPlayer.prepare();
                        mPlayer.seekTo(0);
                    }
                    mPlayer.play();
                }
            }
        });
        return true;
    }
    public boolean pause() {
        if(mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.pause();
            }
        });
        return true;
    }
    public boolean stop() {
        if(mPlayer == null) return false;
        AxmolEngine.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mPlayer != null)
                    mPlayer.stop();
            }
        });
        return true;
    }

    /**
     * Get playback State match with native MEMediaState
     */
    public int getState() {
        return mState;
    }

    /** update video informations */
    private MediaFormat updateVideoInfo() {
        MediaFormat format = mVideoRenderer.getOutputMediaFormat();
        mOutputDim.x = format.getInteger(MediaFormat.KEY_WIDTH);
        if (format.containsKey(MediaFormat.KEY_CROP_LEFT)
            && format.containsKey(MediaFormat.KEY_CROP_RIGHT)) {
            mVideoDim.x = format.getInteger(MediaFormat.KEY_CROP_RIGHT) + 1
                - format.getInteger(MediaFormat.KEY_CROP_LEFT);
        }
        else
            mVideoDim.x = mOutputDim.x;

        mOutputDim.y = format.getInteger(MediaFormat.KEY_HEIGHT);
        if (format.containsKey(MediaFormat.KEY_CROP_TOP)
            && format.containsKey(MediaFormat.KEY_CROP_BOTTOM)) {
            mVideoDim.y = format.getInteger(MediaFormat.KEY_CROP_BOTTOM) + 1
                - format.getInteger(MediaFormat.KEY_CROP_TOP);
        }
        else
            mVideoDim.y = mOutputDim.y;

        mState = STATE_PLAYING;

        return format;
    }

    /** handler or listener methods */

    @Override
    public void handleVideoSample(MediaCodecAdapter codec, int index) {
//        MediaFormat format = updateVideoInfo();

//        String mimeType = format.getString(MediaFormat.KEY_MIME); // "video/raw" (NV12)
//        Integer colorFormat = format.getInteger(MediaFormat.KEY_COLOR_FORMAT);
//        boolean NV12 = colorFormat == AxmolVideoRenderer.DESIRED_PIXEL_FORMAT;

        mState = STATE_PLAYING;
        updateVideoInfo();

        ByteBuffer tmpBuffer = codec.getOutputBuffer(index);
        nativeHandleVideoSample(mNativeObj, tmpBuffer, tmpBuffer.remaining(), mOutputDim.x, mOutputDim.y, mVideoDim.x, mVideoDim.y);
    }

    @Override
    public void onIsPlayingChanged(boolean isPlaying) {
        Log.d(TAG, "[Individual]onIsPlayingChanged: " + isPlaying);
    }

    /**
     *
     * @param playbackState
     *     int STATE_IDLE = 1;
     *     int STATE_BUFFERING = 2;
     *     int STATE_READY = 3;
     *     int STATE_ENDED = 4;
     */
    @Override
    public void onPlaybackStateChanged(int playbackState) {
        Log.d(TAG, "[Individual]onPlaybackStateChanged: " + playbackState);
    }

    @Override
    public void onVideoSizeChanged(VideoSize videoSize) {
        Log.d(TAG, String.format("[Individual]onVideoSizeChanged: (%d,%d)", videoSize.width, videoSize.height));
    }

    public void nativeEvent(int event){
        if(mNativeObj != 0) {
            nativeHandleEvent(mNativeObj, event);
        }
    }

    // @Override
    // public void onRenderedFirstFrame() {
    //     Player.Listener.super.onRenderedFirstFrame();
    // }

    @Override
    public void onEvents(Player player, Player.Events events) {
        if (mPlayer == null) return;

        if(events.contains(Player.EVENT_IS_PLAYING_CHANGED)) {
            Log.d(TAG, "onIsPlayingChanged: " + mPlayer.isPlaying());
            if (mPlayer.isPlaying())
                nativeEvent(EVENT_PLAYING);
            else {
                nativeEvent(EVENT_PAUSED);
                mState = STATE_PAUSED;
            }
        }
        if(events.contains(Player.EVENT_PLAYBACK_STATE_CHANGED)) {
            int state = mPlayer.getPlaybackState();
            Log.d(TAG, "onPlaybackStateChanged: " + state);
            switch (state) {
                case Player.STATE_READY:
//                    mIsH256 = mVideoRenderer.isH2565();
                    break;
                case Player.STATE_ENDED:
                    nativeEvent(EVENT_COMPLETED);
                    mState = STATE_COMPLETED;
                    break;
                case Player.STATE_IDLE:
                    nativeEvent(EVENT_STOPPED);
                    mState = STATE_STOPPED;
                    break;
                default: ;
            }
        }
        if(events.contains(Player.EVENT_PLAYER_ERROR)) {
            Log.e(TAG, "onPlayerError: " + mPlayer.isPlaying());
            nativeEvent(EVENT_ERROR);
            mState = STATE_ERROR;
        }
        if(events.contains(Player.EVENT_VIDEO_SIZE_CHANGED)) {
            final VideoSize videoSize = mPlayer.getVideoSize();
            Log.d(TAG, String.format("onVideoSizeChanged: %d, %d", videoSize.width, videoSize.height));
        }
        if(events.contains(Player.EVENT_IS_LOADING_CHANGED)) {
            Log.d(TAG, "onIsLoadingChanged: " + mPlayer.isLoading());
        }
    }

    final class AxmolRenderersFactory extends DefaultRenderersFactory {

        public AxmolRenderersFactory(Context context) {
            super(context);
            setAllowedVideoJoiningTimeMs(0);
        }

        @Override
        protected void buildVideoRenderers(
            Context context,
            @ExtensionRendererMode int extensionRendererMode,
            MediaCodecSelector mediaCodecSelector,
            boolean enableDecoderFallback,
            Handler eventHandler,
            VideoRendererEventListener eventListener,
            long allowedVideoJoiningTimeMs,
            ArrayList<Renderer> out) {
            out.add(
                new AxmolVideoRenderer(
                    context,
                    getCodecAdapterFactory(),
                    mediaCodecSelector,
                    allowedVideoJoiningTimeMs,
                    enableDecoderFallback,
                    eventHandler,
                    eventListener,
                    MAX_DROPPED_VIDEO_FRAME_COUNT_TO_NOTIFY));
        }
    }
}


