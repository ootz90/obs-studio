/******************************************************************************
    Copyright (C) 2013-2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

/** Specifies the encoder type */
enum obs_encoder_type {
	OBS_ENCODER_AUDIO,
	OBS_ENCODER_VIDEO
};

/** Encoder output packet */
struct encoder_packet {
	uint8_t               *data;        /**< Packet data */
	size_t                size;         /**< Packet size */

	int64_t               pts;          /**< Presentation timestamp */
	int64_t               dts;          /**< Decode timestamp */

	int32_t               timebase_num; /**< Timebase numerator */
	int32_t               timebase_den; /**< Timebase denominator */

	enum obs_encoder_type type;         /**< Encoder type */

	/* ---------------------------------------------------------------- */
	/* Internal video variables (will be parsed automatically) */

	bool                  keyframe;     /**< Is a keyframe */

	/**
	 * Packet priority
	 *
	 * This is generally use by video encoders to specify the priority
	 * of the packet.
	 */
	int                   priority;

	/**
	 * Dropped packet priority
	 *
	 * If this packet needs to be dropped, the next packet must be of this
	 * priority or higher to continue transmission.
	 */
	int                   drop_priority;
};

/** Encoder input frame */
struct encoder_frame {
	/** Data for the frame/audio */
	uint8_t               *data[MAX_AV_PLANES];

	/** size of each plane */
	uint32_t              linesize[MAX_AV_PLANES];

	/** Number of frames (audio only) */
	uint32_t              frames;

	/** Presentation timestamp */
	int64_t               pts;
};

/**
 * Encoder interface
 *
 * Encoders have a limited usage with OBS.  You are not generally supposed to
 * implement every encoder out there.  Generally, these are limited or specific
 * encoders for h264/aac for streaming and recording.  It doesn't have to be
 * *just* h264 or aac of course, but generally those are the expected encoders.
 *
 * That being said, other encoders will be kept in mind for future use.
 */
struct obs_encoder_info {
	/* ----------------------------------------------------------------- */
	/* Required implementation*/

	/** Specifies the named identifier of this encoder */
	const char *id;

	/** Specifies the encoder type (video or audio) */
	enum obs_encoder_type type;

	/** Specifies the codec */
	const char *codec;

	/**
	 * Gets the full translated name of this encoder
	 *
	 * @param  locale  Locale to use for translation
	 * @return         Translated name of the encoder
	 */
	const char *(*getname)(const char *locale);

	/**
	 * Creates the encoder with the specified settings
	 *
	 * @param  settings  Settings for the encoder
	 * @param  encoder   OBS encoder context
	 * @return           Data associated with this encoder context
	 */
	void *(*create)(obs_data_t settings, obs_encoder_t encoder);

	/**
	 * Destroys the encoder data
	 *
	 * @param  data  Data associated with this encoder context
	 */
	void (*destroy)(void *data);

	/**
	 * Initializes the encoder with the specified settings
	 *
	 * @param  data      Data associated with this encoder context
	 * @param  settings  Settings for the encoder
	 * @return           true if the encoder settings are valid and the
	 *                   encoder is ready to be used, false otherwise
	 */
	bool (*initialize)(void *data, obs_data_t settings);

	/**
	 * Encodes frame(s), and outputs encoded packets as they become
	 * available.
	 *
	 * @param       data             Data associated with this encoder
	 *                               context
	 * @param[in]   frame            Raw audio/video data to encode
	 * @param[out]  packet           Encoder packet output, if any
	 * @param[out]  received_packet  Set to true if a packet was received,
	 *                               false otherwise
	 * @return                       true if successful, false otherwise.
	 */
	bool (*encode)(void *data, struct encoder_frame *frame,
			struct encoder_packet *packet, bool *received_packet);

	/* ----------------------------------------------------------------- */
	/* Optional implementation */

	/**
	 * Gets the default settings for this encoder
	 *
	 * @param[out]  settings  Data to assign default settings to
	 */
	void (*defaults)(obs_data_t settings);

	/** 
	 * Gets the property information of this encoder
	 *
	 * @param  locale  The locale to translate with
	 * @return         The properties data
	 */
	obs_properties_t (*properties)(const char *locale);

	/**
	 * Updates the settings for this encoder (usually used for things like
	 * changeing birate while active)
	 *
	 * @param  data      Data associated with this encoder context
	 * @param  settings  New settings for this encoder
	 * @return           true if successful, false otherwise
	 */
	bool (*update)(void *data, obs_data_t settings);

	/**
	 * Returns extra data associated with this encoder (usually header)
	 *
	 * @param  data             Data associated with this encoder context
	 * @param[out]  extra_data  Pointer to receive the extra data
	 * @param[out]  size        Pointer to receive the size of the extra
	 *                          data
	 * @return                  true if extra data available, false
	 *                          otherwise
	 */
	bool (*extra_data)(void *data, uint8_t **extra_data, size_t *size);

	/**
	 * Gets the SEI data, if any
	 *
	 * @param       data      Data associated with this encoder context
	 * @param[out]  sei_data  Pointer to receive the SEI data
	 * @param[out]  size      Pointer to receive the SEI data size
	 * @return                true if SEI data available, false otherwise
	 */
	bool (*sei_data)(void *data, uint8_t **sei_data, size_t *size);

	/**
	 * Returns desired audio format and sample information
	 *
	 * @param       data  Data associated with this encoder context
	 * @param[out]  info  Audio format information
	 * @return            true if specific format is desired, false
	 *                    otherwise
	 */
	bool (*audio_info)(void *data, struct audio_convert_info *info);

	/**
	 * Returns desired video format information
	 *
	 * @param       data  Data associated with this encoder context
	 * @param[out]  info  Video format information
	 * @return            true if specific format is desired, false
	 *                    otherwise
	 */
	bool (*video_info)(void *data, struct video_scale_info *info);
};

/**
 * Register an encoder definition to the current obs context.  This should be
 * used in obs_module_load.
 *
 * @param  info  Pointer to the source definition structure.
 */
EXPORT void obs_register_encoder(const struct obs_encoder_info *info);
