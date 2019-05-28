/********************************** (C) COPYRIGHT *******************************
* File Name          : glucose_clock.h
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/12
* Description        : 
            
*******************************************************************************/

#ifndef GLUCOSE_CLOCK_H
#define GLUCOSE_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

#define	IsLeapYear(yr)	(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC
typedef uint32 UTCTime;

// To be used with
typedef struct
{
  uint8 seconds;  // 0-59
  uint8 minutes;  // 0-59
  uint8 hour;     // 0-23
  uint8 day;      // 0-30
  uint8 month;    // 0-11
  uint16 year;    // 2000+
} UTCTimeStruct;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

  /*
   * Updates the TMOS clock and Timers from the MAC 320us timer tick.
   */
  extern void tmosTimeUpdate( void );

  /*
   * Set the new time.  This will only set the seconds portion
   * of time and doesn't change the factional second counter.
   *     newTime - number of seconds since 0 hrs, 0 minutes,
   *               0 seconds, on the 1st of January 2000 UTC
   */
  extern void tmos_setClock( UTCTime newTime );

  /*
   * Gets the current time.  This will only return the seconds
   * portion of time and doesn't include the factional second counter.
   *     returns: number of seconds since 0 hrs, 0 minutes,
   *              0 seconds, on the 1st of January 2000 UTC
   */
  extern UTCTime tmos_getClock( void );

  /*
   * Converts UTCTime to UTCTimeStruct
   *
   * secTime - number of seconds since 0 hrs, 0 minutes,
   *          0 seconds, on the 1st of January 2000 UTC
   * tm - pointer to breakdown struct
   */
  extern void tmos_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime );

  /*
   * Converts UTCTimeStruct to UTCTime (seconds since 00:00:00 01/01/2000)
   *
   * tm - pointer to UTC time struct
   */
  extern UTCTime tmos_ConvertUTCSecs( UTCTimeStruct *tm );

  /*
   * Update/Adjust the tmos clock and timers
   * Msec - elapsed time in milli seconds 
   */
  extern void tmosAdjustTimer( uint32 Msec );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
