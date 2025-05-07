#ifndef MELP_H
#define MELP_H

#include <stdint.h>

#include "melp/defines.h"
#include "melp/version.h"

/**
 * @file melp.h
 * @brief MELP API
 */

/**
 * @mainpage MELP
 *
 * The characteristics of this MELP voice coder implementation are the
 * following:
 *
 * @li Fixed-point implementation
 * @li 8 kHz sampling rate
 * @li 22.5 ms frame size
 * @li 600, 1200 and 2400 bit/s
 *
 * Documentation sections:
 * @li @ref melp_common
 * @li @ref melp_analyzer
 * @li @ref melp_synthesizer
 */

/**
 * @defgroup melp_common Common
 * @{
 */

/**
 * @enum melp_rate
 *
 * MELP bit rates.
 */
enum melp_rate
{
    MELP_RATE_0600, /**< 600 bit/s */
    MELP_RATE_1200, /**< 1200 bit/s */
    MELP_RATE_2400  /**< 2400 bit/s */
};

/**
 * @defgroup melp_error_codes Error codes
 *@{
 */
/** Invalid argument */
#define MELP_INVAL  -1
/** Not enough memory */
#define MELP_NOMEM  -2
/**@}*/

/**
 * @defgroup melp_sizes Frames sizes
 *@{
 *
 * @brief This page describes the super-frame and bitstream sizes for each
 * bitrate.
 *
 * Both the super-frame and bitstream sizes depend on the bitrate:
 *
 * <table>
 *   <tr>
 *     <th>Bit Rate (bit/s)</th>
 *     <th>Frame size (samples)</th>
 *     <th>Super-frame size (samples)</th>
 *     <th>Bitstream size (bytes)</th>
 *   </tr>
 *   <tr>
 *     <td>600</td>
 *     <td>180</td>
 *     <td>720</td>
 *     <td>7</td>
 *   </tr>
 *   <tr>
 *     <td>1200</td>
 *     <td>180</td>
 *     <td>540</td>
 *     <td>11</td>
 *   </tr>
 *   <tr>
 *     <td>2400</td>
 *     <td>180</td>
 *     <td>180</td>
 *     <td>7</td>
 *   </tr>
 * </table>
 */

/** Number of samples per speech frame */
#define MELP_NB_SAMPLES_PER_FRAME 180

/** Number of frames per super-frame at 600 bit/s */
#define MELP_0600_NB_FRAMES_PER_SUPERFRAME 4
/** Number of frames per super-frame at 1200 bit/s */
#define MELP_1200_NB_FRAMES_PER_SUPERFRAME 3
/** Number of frames per super-frame at 2400 bit/s */
#define MELP_2400_NB_FRAMES_PER_SUPERFRAME 1
/** Maximum number of frames per super-frame */
#define MELP_MAX_NB_FRAMES_PER_SUPERFRAME MELP_0600_NB_FRAMES_PER_SUPERFRAME

/** Number of samples per super-frame at 600 bit/s */
#define MELP_0600_SAMPLES_PER_SUPERFRAME \
        (MELP_NB_SAMPLES_PER_FRAME * MELP_0600_NB_FRAMES_PER_SUPERFRAME)
/** Number of samples per super-frame at 1200 bit/s */
#define MELP_1200_SAMPLES_PER_SUPERFRAME \
        (MELP_NB_SAMPLES_PER_FRAME * MELP_1200_NB_FRAMES_PER_SUPERFRAME)
/** Number of samples per super-frame at 2400 bit/s */
#define MELP_2400_SAMPLES_PER_SUPERFRAME \
        (MELP_NB_SAMPLES_PER_FRAME * MELP_2400_NB_FRAMES_PER_SUPERFRAME)
/** Maximum number of samples per super-frame */
#define MELP_MAX_SAMPLES_PER_SUPERFRAME  MELP_0600_SAMPLES_PER_SUPERFRAME

/** Bitstream size at 600 bit/s */
#define MELP_0600_BITSTREAM_SIZE 7
/** Bitstream size at 1200 bit/s */
#define MELP_1200_BITSTREAM_SIZE 11
/** Bitstream size at 2400 bit/s */
#define MELP_2400_BITSTREAM_SIZE 7
/** Maximum packed bitstream size */
#define MELP_MAX_BITSTREAM_SIZE MELP_1200_BITSTREAM_SIZE
/**@}*/

/**
 * Gets the library version string.
 *
 * @return The version string.
 */
MELP_EXPORT const char *melp_get_version_string(void);

/**
 * Gets the number of frames in a super-frame for a given bitrate.
 *
 * @param rate bit rate
 *
 * @return The number of frames or @ref melp_error_codes
 */
MELP_EXPORT int melp_get_nb_frames(enum melp_rate rate);

/**
 * Gets the bitstream size in bytes for a given bitrate.
 *
 * @param rate bit rate
 *
 * @return The bitstream size or @ref melp_error_codes
 */
MELP_EXPORT int melp_get_bitstream_size(enum melp_rate rate);

/**
 * Gets the number of samples in a super-frame for a given bitrate.
 *
 * @param rate bit rate
 *
 * @return The number of samples or @ref melp_error_codes
 */
MELP_EXPORT int melp_get_nb_samples_per_superframe(enum melp_rate rate);

/**@}*/

/**
 * @defgroup melp_analyzer Analyzer
 * @{
 *
 * @brief This page describes the functions used to perform a MELP analysis.
 *
 * The analysis process starts with creating an analyzer state, as follows:
 *
 * @code
 * int rc = 0;
 * struct melp_analyzer *inst = NULL;
 * rc = melp_analyzer_new(&inst, rate, npp);
 * @endcode
 *
 * Where
 *
 * @arg rate is either #MELP_RATE_0600, #MELP_RATE_1200 or #MELP_RATE_2400
 * @arg npp is either #MELP_NO_NPP or #MELP_NPP
 *
 * At that point, the state @c inst can be used to analyze an audio stream.
 *
 * It is also possible to initialize pre-allocated memory with
 * melp_analyzer_init():
 *
 * @code
 * int size = 0;
 * int rc = 0;
 * struct melp_analyzer *inst = NULL;
 * size = melp_analyzer_get_size(rate, npp);
 * inst = malloc(size);
 * rc = melp_analyzer_init(inst, rate, npp);
 * @endcode
 *
 * Where melp_analyzer_get_size() returns the number of bytes that must be
 * allocated to hold the analyzer state.
 *
 * The analysis itself is performed on a 22.5 ms audio sub-frame with a
 * sampling frequency of 8 kHz. The super-frame size depends on the bit rate:
 *
 * <table>
 *   <tr>
 *     <th>Bit Rate (bit/s)</th>
 *     <th>Frame interval (ms)</th>
 *     <th>Super-frame interval (ms)</th>
 *     <th>Bitstream size (bits)</th>
 *   </tr>
 *   <tr>
 *     <td>600</td>
 *     <td>22.5</td>
 *     <td>90</td>
 *     <td>54</td>
 *   </tr>
 *   <tr>
 *     <td>1200</td>
 *     <td>22.5</td>
 *     <td>67.5</td>
 *     <td>81</td>
 *   </tr>
 *   <tr>
 *     <td>2400</td>
 *     <td>22.5</td>
 *     <td>22.5</td>
 *     <td>54</td>
 *   </tr>
 * </table>
 *
 * melp_analyze() must be called with exactly one frame of
 * #MELP_NB_SAMPLES_PER_FRAME samples. The resulting bitstream is written to the
 * output buffer once a super-frame is complete:
 *
 * @code
 * len = melp_analyze(inst, frame, bitstream);
 * @endcode
 *
 * Where
 *
 * @arg frame is the audio data in PCM signed 16-bit format
 * @arg bitstream is the buffer where the bitstream is written
 *
 * melp_analyze() returns the number of bytes written to the output buffer,
 * zero if the super-frame is incomplete. The return value can be negative, in
 * which case an error has occurred.
 *
 * The analyzer state can be destroyed as follows:
 *
 * @code
 * melp_analyzer_free(inst);
 * @endcode
 *
 * No action is required if the analyzer was created with
 * melp_analyzer_init(), besides releasing the manually allocated memory.
 */

/**
 * MELP analyzer state.
 *
 * @see melp_analyzer_new,melp_analyzer_init
 */
struct melp_analyzer;

/**
 * @enum melp_npp_type
 *
 * MELP noise pre-processing type.
 */
enum melp_npp_type
{
    MELP_NO_NPP, /**< Disable the noise pre-processing */
    MELP_NPP     /**< Enable the noise pre-processing */
};

/**
 * Gets the size of an analyzer state.
 *
 * @param[in] rate bit rate
 * @param[in] npp noise pre-processing type
 *
 * @return The size in bytes or @ref melp_error_codes
 */
MELP_EXPORT int melp_analyzer_get_size(enum melp_rate rate,
                                       enum melp_npp_type npp);

/**
 * Initializes a previously allocated analyzer state.
 *
 * This function is intended for applications which use a custom
 * allocator. The state must be at least the size returned by
 * @ref melp_analyzer_get_size().
 *
 * @param[in] inst analyzer state
 * @param[in] rate bit rate
 * @param[in] npp noise pre-processing type
 *
 * @retval Zero or @ref melp_error_codes
 */
MELP_EXPORT int melp_analyzer_init(struct melp_analyzer *inst,
                                   enum melp_rate rate, enum melp_npp_type npp);

/**
 * Resets the analyzer state to a freshly initialized state.
 *
 * This function should be called when the state is reused to analyze another
 * audio stream.
 *
 * @param[in] inst analyzer state
 */
MELP_EXPORT void melp_analyzer_reset(struct melp_analyzer *inst);

/**
 * Allocates and initializes an analyzer state.
 *
 * @param[in,out] inst analyzer state pointer
 * @param[in] rate bit rate
 * @param[in] npp noise pre-processing type
 *
 * @retval Zero or @ref melp_error_codes
 */
MELP_EXPORT int melp_analyzer_new(struct melp_analyzer **inst,
                                  enum melp_rate rate, enum melp_npp_type npp);

/**
 * Frees an analyzer state allocated by @ref melp_analyzer_new().
 *
 * @param[in] inst analyzer state
 */
MELP_EXPORT void melp_analyzer_free(struct melp_analyzer *inst);

/**
 * Analyzes a speech frame.
 *
 * @param[in] inst analyzer state
 * @param[in] speech input speech frame
 * @param[out] bitstream output bitstream
 *
 * @return The size of the bitstream or @ref melp_error_codes
 *
 * @remark The maximum stack usage is 10 kB for 600 bit/s.
 */
MELP_EXPORT int melp_analyze(struct melp_analyzer *inst, const int16_t *speech,
                             uint8_t *bitstream);

/**@}*/

/**
 * @defgroup melp_synthesizer Synthesizer
 * @{
 *
 * @brief This page describes the functions used to perform a MELP synthesis.
 *
 * The synthesis process starts with creating an synthesizer state, as follows:
 *
 * @code
 * int rc = 0;
 * struct melp_synthesizer *inst = NULL;
 * rc = melp_synthesizer_new(&inst, rate, flt);
 * @endcode
 *
 * Where
 *
 * @arg rate is either #MELP_RATE_0600, #MELP_RATE_1200 or #MELP_RATE_2400
 * @arg flt is either #MELP_NO_POSTFILTER or #MELP_POSTFILTER
 *
 * At that point, the state @c inst can be used to synthesize a bitstream.
 *
 * It is also possible to initialize pre-allocated memory with
 * melp_synthesizer_init():
 *
 * @code
 * int size = 0;
 * int rc = 0;
 * struct melp_synthesizer *inst = NULL;
 * size = melp_synthesizer_get_size(rate, flt);
 * inst = malloc(size);
 * rc = melp_synthesizer_init(inst, rate, flt);
 * @endcode
 *
 * Where melp_synthesizer_get_size() returns the number of bytes that must
 * be allocated to hold the synthesizer state.
 *
 * melp_synthesize() will synthesize exactly one frame of
 * #MELP_NB_SAMPLES_PER_FRAME per call. Depending on the bit rate, it must be
 * called multiple times to synthesize a super-frame from a given bitstream:
 *
 * @code
 * len = melp_synthesize(inst, bitstream, frame);
 * @endcode
 *
 * Where
 *
 * @arg bitstream is the buffer containing the bitstream
 * @arg frame is the synthesized audio data
 *
 * melp_synthesize() returns the number of samples written to the output audio
 * buffer. The return value can be negative, in which case an error has
 * occurred.
 *
 * The synthesizer state can be destroyed as follows:
 *
 * @code
 * melp_synthesizer_free(inst);
 * @endcode
 *
 * No action is required if the synthesizer was created with
 * melp_synthesizer_init(), besides releasing the manually allocated memory.
 */

/**
 * @enum melp_postfilter_type
 *
 * MELP post-filter type.
 */
enum melp_postfilter_type
{
    MELP_NO_POSTFILTER, /**< Disable the post-filter */
    MELP_POSTFILTER     /**< Enable the post-filter */
};

/**
 * MELP synthesizer state.
 *
 * @see melp_synthesizer_new,melp_synthesizer_init
 */
struct melp_synthesizer;

/**
 * Gets the size of a synthesizer state.
 *
 * @param[in] rate bit rate
 * @param[in] flt post-filter type
 *
 * @return The size in bytes or @ref melp_error_codes
 */
MELP_EXPORT int melp_synthesizer_get_size(enum melp_rate rate,
                                          enum melp_postfilter_type flt);

/**
 * Initializes a previously allocated synthesizer state.
 *
 * This function is intended for applications which use a custom
 * allocator. The state must be at least the size returned by
 * @ref melp_synthesizer_get_size().
 *
 * @param[in] inst synthesizer state
 * @param[in] rate bit rate
 * @param[in] flt post-filter type
 *
 * @retval Zero or @ref melp_error_codes
 */
MELP_EXPORT int melp_synthesizer_init(struct melp_synthesizer *inst,
                                      enum melp_rate rate,
                                      enum melp_postfilter_type flt);

/**
 * Resets the synthesizer state to a freshly initialized state.
 *
 * This function should be called when the state is reused to synthesize another
 * bitstream.
 *
 * @param[in] inst synthesizer state
 */
MELP_EXPORT void melp_synthesizer_reset(struct melp_synthesizer *inst);

/**
 * Allocates and initializes a synthesizer state.
 *
 * @param[in,out] inst synthesizer state pointer
 * @param[in] rate bit rate
 * @param[in] flt post-filter type
 *
 * @retval Zero or @ref melp_error_codes
 */
MELP_EXPORT int melp_synthesizer_new(struct melp_synthesizer **inst,
                                     enum melp_rate rate,
                                     enum melp_postfilter_type flt);

/**
 * Frees a synthesizer state allocated by @ref melp_synthesizer_new().
 *
 * @param[in] inst synthesizer state
 */
MELP_EXPORT void melp_synthesizer_free(struct melp_synthesizer *inst);

/**
 * Synthesizes a bitstream.
 *
 * @param[in] inst synthesizer state
 * @param[in] bitstream input bitstream
 * @param[out] speech output speech frame
 *
 * @return The number of samples in the output speech frame or
 * @ref melp_error_codes
 *
 * @remark The maximum stack usage is 5 kB for 600 bit/s.
 */
MELP_EXPORT int melp_synthesize(struct melp_synthesizer *inst,
                                const uint8_t *bitstream, int16_t *speech);

/**@}*/

#endif
