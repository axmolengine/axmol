/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Axmol ByteBuffer-mode video renderer for AndroidX Media3.
 */
package dev.axmol.lib;

import static androidx.media3.common.util.Assertions.checkNotNull;
import static androidx.media3.common.util.Util.msToUs;
import static androidx.media3.exoplayer.DecoderReuseEvaluation.DISCARD_REASON_MAX_INPUT_SIZE_EXCEEDED;
import static androidx.media3.exoplayer.DecoderReuseEvaluation.DISCARD_REASON_VIDEO_MAX_RESOLUTION_EXCEEDED;
import static androidx.media3.exoplayer.DecoderReuseEvaluation.REUSE_RESULT_NO;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo.CodecCapabilities;
import android.media.MediaCrypto;
import android.media.MediaFormat;
import android.os.Handler;

import androidx.annotation.Nullable;
import androidx.media3.common.C;
import androidx.media3.common.Format;
import androidx.media3.common.util.TraceUtil;
import androidx.media3.common.util.UnstableApi;
import androidx.media3.exoplayer.DecoderReuseEvaluation;
import androidx.media3.exoplayer.ExoPlaybackException;
import androidx.media3.exoplayer.mediacodec.MediaCodecAdapter;
import androidx.media3.exoplayer.mediacodec.MediaCodecInfo;
import androidx.media3.exoplayer.mediacodec.MediaCodecSelector;
import androidx.media3.exoplayer.source.MediaSource;
import androidx.media3.exoplayer.video.VideoFrameMetadataListener;
import androidx.media3.exoplayer.video.VideoRendererEventListener;

import java.nio.ByteBuffer;

/**
 * Decodes video with {@link MediaCodec} and exposes decoder output buffers to Axmol.
 */
@UnstableApi
public class ByteBufferVideoRenderer
    extends androidx.media3.exoplayer.video.MediaCodecVideoRenderer {
    public interface VideoFrameProcessor {
        /**
         * Processes a decoded frame on the playback thread.
         *
         * <p>The implementation must consume or copy the frame synchronously before returning. The
         * renderer releases the codec output buffer immediately after this callback returns.
         */
        void processVideoFrame(MediaCodecAdapter codec, int index, long presentationTimeUs);
    }

    /**
     * Desired decoder output pixel format: NV12. The renderer falls back to flexible YUV if a codec
     * reports that it does not support this exact format.
     */
    public static final int DESIRED_PIXEL_FORMAT = CodecCapabilities.COLOR_FormatYUV420SemiPlanar;
    private static final int FALLBACK_PIXEL_FORMAT = CodecCapabilities.COLOR_FormatYUV420Flexible;

    private static final long MAX_EARLY_US_THRESHOLD = 50_000;

    @Nullable
    private volatile VideoFrameProcessor output;
    @Nullable
    private VideoFrameMetadataListener frameMetadataListener;
    // Mirrors MediaCodecVideoRenderer's private codecMaxValues field because this renderer provides
    // its own codec configuration and reuse checks.
    @Nullable
    private CodecMaxValues codecMaxValues;

    private long initialPositionUs;
    private long lastRenderRealtimeUs;
    private long lastPresentationTimeUs;
    private boolean hasRenderedFrameSinceReset;

    @SuppressWarnings("deprecation")
    public ByteBufferVideoRenderer(Context context, MediaCodecSelector mediaCodecSelector) {
        super(context, mediaCodecSelector);
        resetAxmolTimingState();
    }

    @SuppressWarnings("deprecation")
    public ByteBufferVideoRenderer(
        Context context, MediaCodecSelector mediaCodecSelector, long allowedJoiningTimeMs) {
        super(context, mediaCodecSelector, allowedJoiningTimeMs);
        resetAxmolTimingState();
    }

    @SuppressWarnings("deprecation")
    public ByteBufferVideoRenderer(
        Context context,
        MediaCodecSelector mediaCodecSelector,
        long allowedJoiningTimeMs,
        @Nullable Handler eventHandler,
        @Nullable VideoRendererEventListener eventListener,
        int maxDroppedFramesToNotify) {
        super(
            context,
            mediaCodecSelector,
            allowedJoiningTimeMs,
            eventHandler,
            eventListener,
            maxDroppedFramesToNotify);
        resetAxmolTimingState();
    }

    @SuppressWarnings("deprecation")
    public ByteBufferVideoRenderer(
        Context context,
        MediaCodecSelector mediaCodecSelector,
        long allowedJoiningTimeMs,
        boolean enableDecoderFallback,
        @Nullable Handler eventHandler,
        @Nullable VideoRendererEventListener eventListener,
        int maxDroppedFramesToNotify) {
        super(
            context,
            mediaCodecSelector,
            allowedJoiningTimeMs,
            enableDecoderFallback,
            eventHandler,
            eventListener,
            maxDroppedFramesToNotify);
        resetAxmolTimingState();
    }

    @SuppressWarnings("deprecation")
    public ByteBufferVideoRenderer(
        Context context,
        MediaCodecAdapter.Factory codecAdapterFactory,
        MediaCodecSelector mediaCodecSelector,
        long allowedJoiningTimeMs,
        boolean enableDecoderFallback,
        @Nullable Handler eventHandler,
        @Nullable VideoRendererEventListener eventListener,
        int maxDroppedFramesToNotify) {
        super(
            context,
            codecAdapterFactory,
            mediaCodecSelector,
            allowedJoiningTimeMs,
            enableDecoderFallback,
            eventHandler,
            eventListener,
            maxDroppedFramesToNotify);
        resetAxmolTimingState();
    }

    @SuppressWarnings("deprecation")
    public ByteBufferVideoRenderer(
        Context context,
        MediaCodecAdapter.Factory codecAdapterFactory,
        MediaCodecSelector mediaCodecSelector,
        long allowedJoiningTimeMs,
        boolean enableDecoderFallback,
        @Nullable Handler eventHandler,
        @Nullable VideoRendererEventListener eventListener,
        int maxDroppedFramesToNotify,
        float assumedMinimumCodecOperatingRate) {
        super(
            context,
            codecAdapterFactory,
            mediaCodecSelector,
            allowedJoiningTimeMs,
            enableDecoderFallback,
            eventHandler,
            eventListener,
            maxDroppedFramesToNotify,
            assumedMinimumCodecOperatingRate);
        resetAxmolTimingState();
    }

    public String getCodecName() {
        @Nullable MediaCodecInfo info = getCodecInfo();
        return info != null ? info.name : "";
    }

    public void setOutput(@Nullable Object output) {
        this.output = output instanceof VideoFrameProcessor ? (VideoFrameProcessor) output : null;
    }

    @Override
    public void handleMessage(@MessageType int messageType, @Nullable Object message)
        throws ExoPlaybackException {
        if (messageType == MSG_SET_VIDEO_OUTPUT && !(message instanceof android.view.Surface)) {
            setOutput(message);
            return;
        }
        if (messageType == MSG_SET_VIDEO_FRAME_METADATA_LISTENER) {
            frameMetadataListener = (VideoFrameMetadataListener) message;
            return;
        }
        super.handleMessage(messageType, message);
    }

    @Override
    public boolean isReady() {
        return super.isReady() || isReadyForDecoding();
    }

    @Override
    protected void onStreamChanged(
        Format[] formats,
        long startPositionUs,
        long offsetUs,
        MediaSource.MediaPeriodId mediaPeriodId)
        throws ExoPlaybackException {
        super.onStreamChanged(formats, startPositionUs, offsetUs, mediaPeriodId);
        resetAxmolTimingState();
    }

    @Override
    protected void onProcessedStreamChange() {
        super.onProcessedStreamChange();
        resetAxmolTimingState();
    }

    @Override
    protected void onPositionReset(long positionUs, boolean joining, boolean suppressJoining)
        throws ExoPlaybackException {
        super.onPositionReset(positionUs, joining, suppressJoining);
        resetAxmolTimingState();
    }

    @Override
    protected void resetCodecStateForFlush() {
        super.resetCodecStateForFlush();
        resetAxmolTimingState();
    }

    @Override
    protected boolean shouldInitCodec(MediaCodecInfo codecInfo) {
        return true;
    }

    @Override
    protected DecoderReuseEvaluation canReuseCodec(
        MediaCodecInfo codecInfo, Format oldFormat, Format newFormat, boolean isAdaptiveFormatChange) {
        DecoderReuseEvaluation evaluation = codecInfo.canReuseCodec(oldFormat, newFormat);

        int discardReasons = evaluation.discardReasons;
        @Nullable CodecMaxValues codecMaxValues = this.codecMaxValues;
        if (codecMaxValues != null) {
            if (newFormat.width > codecMaxValues.width || newFormat.height > codecMaxValues.height) {
                discardReasons |= DecoderReuseEvaluation.DISCARD_REASON_VIDEO_MAX_RESOLUTION_EXCEEDED;
            }
            if (getMaxInputSize(codecInfo, newFormat) > codecMaxValues.inputSize) {
                discardReasons |= DecoderReuseEvaluation.DISCARD_REASON_MAX_INPUT_SIZE_EXCEEDED;
            }
        }

        return new DecoderReuseEvaluation(
            codecInfo.name,
            oldFormat,
            newFormat,
            discardReasons != 0 ? DecoderReuseEvaluation.REUSE_RESULT_NO : evaluation.result,
            discardReasons);
    }

    @Override
    protected MediaCodecAdapter.Configuration getMediaCodecConfiguration(
        MediaCodecInfo codecInfo,
        Format format,
        @Nullable MediaCrypto crypto,
        float codecOperatingRate) {
        String codecMimeType = codecInfo.codecMimeType;
        CodecMaxValues codecMaxValues = getCodecMaxValues(codecInfo, format, getStreamFormats());
        this.codecMaxValues = codecMaxValues;
        MediaFormat mediaFormat =
            getMediaFormat(
                format,
                codecMimeType,
                codecMaxValues,
                codecOperatingRate,
                /* deviceNeedsNoPostProcessWorkaround= */ false,
                C.AUDIO_SESSION_ID_UNSET);
        int colorFormat = getSupportedOutputColorFormat(codecInfo);
        if (colorFormat != Format.NO_VALUE) {
            mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, colorFormat);
        }
        mediaFormat.setInteger(MediaFormat.KEY_ALLOW_FRAME_DROP, 0);
        return MediaCodecAdapter.Configuration.createForVideoDecoding(
            codecInfo,
            mediaFormat,
            format,
            /* surface= */ null,
            crypto);
    }

    @Override
    protected boolean processOutputBuffer(
        long positionUs,
        long elapsedRealtimeUs,
        @Nullable MediaCodecAdapter codec,
        @Nullable ByteBuffer buffer,
        int bufferIndex,
        int bufferFlags,
        int sampleCount,
        long bufferPresentationTimeUs,
        boolean isDecodeOnlyBuffer,
        boolean isLastBuffer,
        Format format)
        throws ExoPlaybackException {
        checkNotNull(codec);

        long presentationTimeUs = bufferPresentationTimeUs - getOutputStreamOffsetUs();
        if (lastPresentationTimeUs != C.TIME_UNSET && presentationTimeUs < lastPresentationTimeUs) {
            resetAxmolTimingState();
        }
        lastPresentationTimeUs = presentationTimeUs;

        if (initialPositionUs == C.TIME_UNSET) {
            initialPositionUs = positionUs;
        }

        if (isDecodeOnlyBuffer && !isLastBuffer) {
            skipOutputBuffer(codec, bufferIndex, presentationTimeUs);
            return true;
        }

        boolean isStarted = getState() == STATE_STARTED;
        long earlyUs = calculateEarlyTimeUs(positionUs, elapsedRealtimeUs, bufferPresentationTimeUs);
        long elapsedSinceLastRenderUs =
            lastRenderRealtimeUs == C.TIME_UNSET ? Long.MAX_VALUE : elapsedRealtimeUs - lastRenderRealtimeUs;

        if (shouldForceRenderOutputBuffer(earlyUs, elapsedSinceLastRenderUs)) {
            renderOutputBuffer(codec, bufferIndex, presentationTimeUs, format);
            updateVideoFrameProcessingOffsetCounters(earlyUs);
            return true;
        }

        if (isStarted && !hasRenderedFrameSinceReset) {
            renderOutputBuffer(codec, bufferIndex, presentationTimeUs, format);
            updateVideoFrameProcessingOffsetCounters(earlyUs);
            return true;
        }

        if (!isStarted || positionUs == initialPositionUs) {
            return false;
        }

        if (earlyUs > MAX_EARLY_US_THRESHOLD) {
            return false;
        }

        renderOutputBuffer(codec, bufferIndex, presentationTimeUs, format);
        updateVideoFrameProcessingOffsetCounters(earlyUs);
        return true;
    }

    @Override
    protected void renderOutputBufferV21(
        MediaCodecAdapter codec, int index, long presentationTimeUs, long releaseTimeNs) {
        // ByteBuffer mode does not render to a Surface, so the base class' Surface-oriented
        // first-frame/video-size notifications are intentionally skipped.
        renderOutputBufferInternal(codec, index, presentationTimeUs);
    }

    private void renderOutputBuffer(
        MediaCodecAdapter codec, int index, long presentationTimeUs, Format format) {
        @Nullable VideoFrameProcessor output = this.output;
        try {
            if (output != null) {
                output.processVideoFrame(codec, index, presentationTimeUs);
            }
            long releaseTimeNs = getClock().nanoTime();
            notifyFrameMetadataListener(presentationTimeUs, releaseTimeNs, format);
        } finally {
            renderOutputBufferInternal(codec, index, presentationTimeUs);
        }
    }

    private void renderOutputBufferInternal(
        MediaCodecAdapter codec, int index, long presentationTimeUs) {
        TraceUtil.beginSection("releaseOutputBuffer");
        codec.releaseOutputBuffer(index, false);
        TraceUtil.endSection();
        decoderCounters.renderedOutputBufferCount++;
        lastRenderRealtimeUs = msToUs(getClock().elapsedRealtime());
        hasRenderedFrameSinceReset = true;
    }

    private void notifyFrameMetadataListener(
        long presentationTimeUs, long releaseTimeNs, Format format) {
        if (frameMetadataListener != null) {
            frameMetadataListener.onVideoFrameAboutToBeRendered(
                presentationTimeUs, releaseTimeNs, format, getCodecOutputMediaFormat());
        }
    }

    private long calculateEarlyTimeUs(
        long positionUs, long elapsedRealtimeUs, long framePresentationTimeUs) {
        long earlyUs = (long) ((framePresentationTimeUs - positionUs) / (double) getPlaybackSpeed());
        if (getState() == STATE_STARTED) {
            earlyUs -= msToUs(getClock().elapsedRealtime()) - elapsedRealtimeUs;
        }
        return earlyUs;
    }

    private void resetAxmolTimingState() {
        initialPositionUs = C.TIME_UNSET;
        lastRenderRealtimeUs = C.TIME_UNSET;
        lastPresentationTimeUs = C.TIME_UNSET;
        hasRenderedFrameSinceReset = false;
    }

    private static int getSupportedOutputColorFormat(MediaCodecInfo codecInfo) {
        @Nullable CodecCapabilities capabilities = codecInfo.capabilities;
        if (capabilities == null || supportsColorFormat(capabilities, DESIRED_PIXEL_FORMAT)) {
            return DESIRED_PIXEL_FORMAT;
        }
        return supportsColorFormat(capabilities, FALLBACK_PIXEL_FORMAT)
            ? FALLBACK_PIXEL_FORMAT
            // Let the codec choose its output format. Consumers should inspect the codec output
            // MediaFormat if they need to handle additional layouts.
            : Format.NO_VALUE;
    }

    private static boolean supportsColorFormat(CodecCapabilities capabilities, int colorFormat) {
        for (int supportedColorFormat : capabilities.colorFormats) {
            if (supportedColorFormat == colorFormat) {
                return true;
            }
        }
        return false;
    }
}
