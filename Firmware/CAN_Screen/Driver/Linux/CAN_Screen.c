// SPDX-License-Identifier: GPL-2.0+

#include <linux/device.h>
#include <linux/usb.h>
#include <linux/hid.h>
#include <linux/hid-sensor-ids.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/hwmon.h>
#include <linux/jiffies.h>
#include <asm/unaligned.h>

MODULE_LICENSE ( "GPL" );
MODULE_AUTHOR ( "Black_Wolf" );
MODULE_DESCRIPTION ( "hwmon driver for CAN Screen - Part of Watercooling system !" );

#define DRIVER_NAME							"same51g19_canscreen"
#define CANSCREEN_VENDOR_ID						0xFFFF
#define CANSCREEN_PRODUCT_ID						0x0E51
#define CANSCREEN_GROUP_ID						0xF100

#define DISCONNECTED_SENSOR						(s16) 0x8000
#define DISCONNECTED_FLOW						(s16) 0xFFFF
#define PULSE_PER_LITER							169
#define PULSE_PER_TRS							2

#define SENSORSDEVICE_TEMPERATURE_ID					1
#define SENSORSDEVICE_MONITOR_ID					2
#define HIGHFLOW2DEVICE_TEMPERATURE_ID					3
#define HIGHFLOW2DEVICE_FLOW_ID						4

#define BOARD_NAME							"CAN Screen board"
#define SENSORSDEVICE_NAME						"CAN Screen - Sensors"
#define HIGHFLOW2DEVICE_NAME						"AquaComputer High-Flow 2"
#define SCREENDEVICE_NAME						"CAN Screen - v1"

#define SENSORS_INTERFACE						2
#define HIGHFLOW2_INTERFACE						3
#define SCREEN_INTERFACE						4

static DEFINE_MUTEX ( canscreen_ep0_lock );

enum hid_sensor_powerstate {
	SENSOR_POWER_STATE_UNDEFINED,
	SENSOR_POWER_STATE_D0_FULL_POWER,
	SENSOR_POWER_STATE_D1_LOW_POWER,
	SENSOR_POWER_STATE_D2_STANBY_POWER_WITH_WAKEUP,
	SENSOR_POWER_STATE_D3_SLEEP_WITH_WAKEUP,
	SENSOR_POWER_STATE_D4_POWER_OFF
};

enum hid_sensor_status {
	SENSOR_STATE_UNDEFINED,
	SENSOR_STATE_READY,
	SENSOR_STATE_NA,
	SENSOR_STATE_ND,
	SENSOR_STATE_INITIALIZING,
	SENSOR_STATE_ACCESS_DENIED,
	SENSOR_STATE_ERROR
};

static const char canscreen_dev_name [] = { 'S', 'A', 'M', 'E', '5', '1', 'G', '1', '9', ' ', 'C', 'A', 'N', ' ', 'S', 'c', 'r', 'e', 'e', 'n' };

static const char *const canscreen_SensorsDevice_temp_labels [] = {
	"ATSAME51G19 Temperature",
	"Case Temperature"
};

static const char *const canscreen_SensorsDevice_in_labels [] = {
	"ATSAME51G19 +1.2V Core Power",
	"ATSAME51G19 +3V Backup Power",
	"ATSAME51G19 +3.3V I/O Power",
	"+3.3V Analog Reference",
	"+5V Board Power",
	"+3.3V Board Power",
	"Ground Noise"
};

static const char *const canscreen_HighFlow2Device_labels [] = {
	"Liquid Temperature",
	"Liquid Flow",
	"Impeller Speed"
};

static int hid_raw_to_milli_degree_celsius_convert ( s16 data ) {
	if ( data == DISCONNECTED_SENSOR ) return -ENODATA;
	return ( data * 10 );
}

static int hid_raw_to_liter_per_hour_convert ( s16 data ) {
	if ( data == DISCONNECTED_FLOW ) return -ENODATA;
	return data;
}

static int hid_raw_to_speed_convert ( s16 data ) {
	if ( data == DISCONNECTED_FLOW ) return -ENODATA;
	const int divisor = 60 * PULSE_PER_TRS;
	int dividend = data * PULSE_PER_LITER;
	return ( dividend + ( divisor / 2 ) ) / divisor;
}

//BEGIN - Les structures de données.
struct canscreen_SensorsFeatures {
	u8 Connection;
	u8 PowerState;
	u8 ReportState;
	u8 Status;
	__le32 Interval;
	__le16 Sensivity;
} __packed;
//END

struct canscreen_SensorDevice {
	struct hid_device *hdev;
	struct device *hwmon_dev;
	struct mutex mutex;
	spinlock_t spinlock;
	struct work_struct feature_work;

	struct canscreen_SensorsFeatures in_features;
	struct canscreen_SensorsFeatures temp_features;

	enum hwmon_sensor_types pending_type;
	u8 pending_report_id;
	bool connected;

	unsigned long updated_in;
	unsigned long updated_temp;

	int temp_input [ 2 ];
	u16 in_input [ 7 ];

	const char *const *temp_label;
	const char *const *in_label;
};

struct canscreen_HighFlow2Device {
	struct hid_device *hdev;
	struct device *hwmon_dev;
	struct mutex mutex;
	spinlock_t spinlock;
	struct work_struct feature_work;
	
	struct canscreen_SensorsFeatures flow_features;
	struct canscreen_SensorsFeatures temp_features;
	
	enum hwmon_sensor_types pending_type;
	u8 pending_report_id;
	bool connected;
	
	unsigned long updated_flow;
	unsigned long updated_temp;
	
	int temp_input [ 1 ];
	int flow_input [ 2 ];
	
	const char *const *temp_label;
	const char *const *flow_label;
};

//BEGIN Routines pour le fonctionnement de HWMON.
static umode_t hwmon_canscreen_SensorsDevice_is_visible ( const void *data, enum hwmon_sensor_types type, u32 attr, int channel ) {
	//const struct canscreen_SensorDevice *sensors = data;

	switch ( type ) {
		case hwmon_temp:
			if ( channel < 2 ) {
				if ( attr == hwmon_temp_input || attr == hwmon_temp_label ) return 0444;
			}
			break;
		case hwmon_in:
			if ( channel < 7 ) {
				if ( attr == hwmon_in_input || attr == hwmon_in_label ) return 0444;
			}
			break;
		default:
			break;
	}

	return 0;
}

static umode_t hwmon_canscreen_HighFlow2Device_is_visible ( const void *data, enum hwmon_sensor_types type, u32 attr, int channel ) {
	//const struct canscreen_HighFlow2Device *highflow2 = data;

	switch ( type ) {
		case hwmon_temp:
			if ( channel < 1 ) {
				if ( attr == hwmon_temp_input || attr == hwmon_temp_label ) return 0444;
			}
			break;
		case hwmon_fan:
			if ( channel < 2 ) {
				if ( attr == hwmon_fan_input || attr == hwmon_fan_label ) return 0444;
			}
			break;
		default:
			break;
	}

	return 0;
}

static int hwmon_canscreen_SensorDevice_read ( struct device *dev, enum hwmon_sensor_types type, u32 attr, int channel, long *val ) {
	struct canscreen_SensorDevice *sensors = dev_get_drvdata ( dev );
	unsigned long flags;
	int ret = 0;

	if ( !sensors ) return -ENODEV;

	spin_lock_irqsave ( &sensors -> spinlock, flags );
	switch ( type ) {
		case hwmon_temp:
			if ( time_after ( jiffies, sensors -> updated_temp + msecs_to_jiffies ( sensors -> temp_features.Interval ) ) ) {
				if ( sensors -> connected ) {
					sensors -> pending_type = type;
					sensors -> pending_report_id = SENSORSDEVICE_TEMPERATURE_ID;
					schedule_work ( &sensors -> feature_work );
				}
				ret = -EAGAIN;
				break;
			}

			switch ( attr ) {
				case hwmon_temp_input:
					if ( sensors -> temp_input [ channel ] == -ENODATA ) {
						ret = -ENODATA;
						break;
					}
					*val = sensors -> temp_input [ channel ];
					break;
				case hwmon_temp_fault:
					*val = ( sensors -> temp_input [ channel ] == -ENODATA );
					break;
				case hwmon_temp_min:
				case hwmon_temp_max:
				case hwmon_temp_crit:
				case hwmon_temp_lcrit:
				case hwmon_temp_alarm:
				case hwmon_temp_min_alarm:
				case hwmon_temp_crit_alarm:
				case hwmon_temp_lcrit_alarm:
					ret = -EOPNOTSUPP;
					break;
				default:
					ret = -EOPNOTSUPP;
					break;
			}
			break;
		case hwmon_in:
			if ( time_after ( jiffies, sensors -> updated_in + msecs_to_jiffies ( sensors -> in_features.Interval ) ) ) {
				if ( sensors -> connected ) {
					sensors -> pending_type = type;
					sensors -> pending_report_id = SENSORSDEVICE_MONITOR_ID;
					schedule_work ( &sensors -> feature_work );
				}
				ret = -EAGAIN;
				break;
			}

			switch ( attr ) {
				case hwmon_in_input:
					*val = sensors -> in_input [ channel ];
					break;
				case hwmon_in_min:
				case hwmon_in_max:
				case hwmon_in_crit:
				case hwmon_in_lcrit:
				case hwmon_in_alarm:
				case hwmon_in_min_alarm:
				case hwmon_in_crit_alarm:
				case hwmon_in_lcrit_alarm:
					ret = -EOPNOTSUPP;
					break;
				default:
					ret = -EOPNOTSUPP;
					break;
			}
			break;
		default:
			ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore ( &sensors -> spinlock, flags );

	return ret;
}

static int hwmon_canscreen_HighFlow2Device_read ( struct device *dev, enum hwmon_sensor_types type, u32 attr, int channel, long *val ) {
	struct canscreen_HighFlow2Device *highflow2 = dev_get_drvdata ( dev );
	unsigned long flags;
	int ret = 0;

	if ( !highflow2 ) return -ENODEV;

	spin_lock_irqsave ( &highflow2 -> spinlock, flags );
	switch ( type ) {
		case hwmon_temp:
			if ( time_after ( jiffies, highflow2 -> updated_temp + msecs_to_jiffies ( highflow2 -> temp_features.Interval ) ) ) {
				if ( highflow2 -> connected ) {
					highflow2 -> pending_type = type;
					highflow2 -> pending_report_id = HIGHFLOW2DEVICE_TEMPERATURE_ID;
					schedule_work ( &highflow2 -> feature_work );
				}
				ret = -EAGAIN;
				break;
			}

			switch ( attr ) {
				case hwmon_temp_input:
					if ( highflow2 -> temp_input [ channel ] == -ENODATA ) {
						ret = -ENODATA;
						break;
					}
					*val = highflow2 -> temp_input [ channel ];
					break;
				case hwmon_temp_min:
				case hwmon_temp_max:
				case hwmon_temp_crit:
				case hwmon_temp_lcrit:
				case hwmon_temp_alarm:
				case hwmon_temp_min_alarm:
				case hwmon_temp_crit_alarm:
				case hwmon_temp_lcrit_alarm:
					ret = -EOPNOTSUPP;
					break;
				default:
					ret = -EOPNOTSUPP;
					break;
			}
			break;
		case hwmon_fan:
			if ( time_after ( jiffies, highflow2 -> updated_flow + msecs_to_jiffies ( highflow2 -> flow_features.Interval ) ) ) {
				if ( highflow2 -> connected ) {
					highflow2 -> pending_type = type;
					highflow2 -> pending_report_id = HIGHFLOW2DEVICE_FLOW_ID;
					schedule_work ( &highflow2 -> feature_work );
				}
				ret = -EAGAIN;
				break;
			}

			switch ( attr ) {
				case hwmon_fan_input:
					*val = highflow2 -> flow_input [ channel ];
					break;
				case hwmon_fan_fault:
					*val = ( highflow2 -> flow_input [ channel ] == -ENODATA );
					break;
				case hwmon_fan_min:
				case hwmon_fan_max:
				case hwmon_fan_alarm:
				case hwmon_fan_min_alarm:
				case hwmon_fan_max_alarm:
					ret = -EOPNOTSUPP;
					break;
				default:
					ret = -EOPNOTSUPP;
					break;
			}
			break;
		default:
			ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore ( &highflow2 -> spinlock, flags );

	return ret;
}

static int hwmon_canscreen_SensorDevice_read_string ( struct  device *dev, enum hwmon_sensor_types type, u32 attr, int channel, const char **str ) {
	struct canscreen_SensorDevice *sensors = dev_get_drvdata ( dev );

	if ( !sensors ) return -ENODEV;

	switch ( type ) {
		case hwmon_temp:
			if ( attr != hwmon_temp_label ) return -EOPNOTSUPP;
			if ( !sensors -> temp_label || !sensors -> temp_label [ channel ] ) return -EINVAL;
			*str = sensors -> temp_label [ channel ];
			break;
		case hwmon_in:
			if ( attr != hwmon_in_label ) return -EOPNOTSUPP;
			if ( !sensors -> in_label || !sensors -> in_label [ channel ] ) return -EINVAL;
			*str = sensors -> in_label [ channel ];
			break;
		default:
			return -EOPNOTSUPP;
	}

	return 0;
}

static int hwmon_canscreen_HighFlow2Device_read_string ( struct  device *dev, enum hwmon_sensor_types type, u32 attr, int channel, const char **str ) {
	struct canscreen_HighFlow2Device *highflow2 = dev_get_drvdata ( dev );

	if ( !highflow2 ) return -ENODEV;

	switch ( type ) {
		case hwmon_temp:
			if ( attr != hwmon_temp_label ) return -EOPNOTSUPP;
			if ( channel == 0 ) {
				*str = canscreen_HighFlow2Device_labels [ 0 ];
				return 0;
			}
			return -EINVAL;
		case hwmon_fan:
			if ( attr != hwmon_fan_label ) return -EOPNOTSUPP;
			if ( channel == 0 || channel == 1 ) {
				*str = canscreen_HighFlow2Device_labels [ channel + 1 ];
				return 0;
			}
			return -EINVAL;
		default:
			return -EOPNOTSUPP;
	}
}

static const struct hwmon_ops hwmon_canscreen_SensorsDevice_ops = {
	.is_visible = hwmon_canscreen_SensorsDevice_is_visible,
	.read = hwmon_canscreen_SensorDevice_read,
	.read_string = hwmon_canscreen_SensorDevice_read_string
};

static const struct hwmon_ops hwmon_canscreen_HighFlow2Device_ops = {
	.is_visible = hwmon_canscreen_HighFlow2Device_is_visible,
	.read = hwmon_canscreen_HighFlow2Device_read,
	.read_string = hwmon_canscreen_HighFlow2Device_read_string
};

static const struct hwmon_channel_info *const hwmon_canscreen_SensorsDevice_info [] = {
	HWMON_CHANNEL_INFO (	temp,
				HWMON_T_MIN | HWMON_T_MAX | HWMON_T_CRIT | HWMON_T_LCRIT |
				HWMON_T_ALARM | HWMON_T_MIN_ALARM | HWMON_T_CRIT_ALARM | HWMON_T_LCRIT_ALARM |
				HWMON_T_INPUT | HWMON_T_LABEL,						// Température du µC ( ATSAME51G19A )
				HWMON_T_MIN | HWMON_T_MAX | HWMON_T_CRIT | HWMON_T_LCRIT |
				HWMON_T_ALARM | HWMON_T_MIN_ALARM | HWMON_T_CRIT_ALARM | HWMON_T_LCRIT_ALARM |
				HWMON_T_INPUT | HWMON_T_LABEL | HWMON_T_FAULT ),			// Sonde de température du boîtier
	HWMON_CHANNEL_INFO (	in,
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL,						// Tension +1,2V de cœur ( ATSAME51G19A )
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL,						// Tension +3V de la pile de backup ( ATSAME51G19A )
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL,						// Tension +3,3V sur les broches I/O ( ATSAME51G19A )
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL,						// Tension +3,3V de référence
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL,						// Tension +5V d'alimentation de la carte
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL,						// Tension +3,3V d'alimentation
				HWMON_I_MIN | HWMON_I_MAX | HWMON_I_CRIT | HWMON_I_LCRIT |
				HWMON_I_ALARM | HWMON_I_MIN_ALARM | HWMON_I_CRIT_ALARM | HWMON_I_LCRIT_ALARM |
				HWMON_I_INPUT | HWMON_I_LABEL ),					//Tension de déchet sur la masse
	NULL
};

static const struct hwmon_channel_info *const hwmon_canscreen_HighFlow2Device_info [] = {
	HWMON_CHANNEL_INFO (	temp,
				HWMON_T_MIN | HWMON_T_MAX | HWMON_T_CRIT | HWMON_T_LCRIT |
				HWMON_T_ALARM | HWMON_T_MIN_ALARM | HWMON_T_CRIT_ALARM | HWMON_T_LCRIT_ALARM |
				HWMON_T_INPUT | HWMON_T_LABEL ),						// Sonde de température de liquide ( AquaComputer HighFlow 2 )
	HWMON_CHANNEL_INFO (	fan,
				HWMON_F_MIN | HWMON_F_MAX | HWMON_F_ALARM | HWMON_F_MIN_ALARM | HWMON_F_MAX_ALARM |
				HWMON_F_INPUT | HWMON_F_LABEL | HWMON_F_FAULT,					// Capteur de débit ( AquaComputer HighFlow 2 )
				HWMON_F_INPUT | HWMON_F_LABEL | HWMON_F_FAULT ),				// Vitesse de rotation du capteur de débit ( Informatif )
	NULL
};

static const struct hwmon_chip_info canscreen_SensorsDevice_chip_info = {
	.ops = &hwmon_canscreen_SensorsDevice_ops,
	.info = hwmon_canscreen_SensorsDevice_info
};

static const struct hwmon_chip_info canscreen_HighFlow2Device_chip_info = {
	.ops = &hwmon_canscreen_HighFlow2Device_ops,
	.info = hwmon_canscreen_HighFlow2Device_info
};
//END



//BEGIN - Routines pour les FEATURES HID.
static int canscreen_recv_report ( struct hid_device *hdev, const u8 id, u8 *data, int size ) {
	u8 *buffer;
	int ret;
	int alloc_size = size + 1;

	buffer = kzalloc ( alloc_size, GFP_KERNEL );
	if ( !buffer ) return -ENOMEM;

	buffer [ 0 ] = id;
	ret = hid_hw_raw_request ( hdev, id, buffer, alloc_size, HID_FEATURE_REPORT, HID_REQ_GET_REPORT );
	if ( ret > 1 ) memcpy ( data, buffer + 1, min ( size, ret - 1 ) );
	kfree ( buffer );

	return ret;
}

static int canscreen_send_report ( struct hid_device *hdev, const u8 id, u8 *data, int size ) {
	u8 *buffer;
	unsigned int retries = 8;
	int ret;
	int alloc_size = size + 1;
	//int alloc_size = 64;

	buffer = kzalloc ( alloc_size, GFP_KERNEL );
	if ( !buffer ) return -ENOMEM;

	buffer [ 0 ] = id;
	memcpy ( buffer + 1, data, size );
	do {
		ret = hid_hw_raw_request ( hdev, id, buffer, alloc_size, HID_FEATURE_REPORT, HID_REQ_SET_REPORT );
		if ( ret != -EPIPE ) break;
		msleep ( 20 );
	} while ( -- retries );

	kfree ( buffer );

	if ( ret < 0 ) hid_err ( hdev, "%s: error %d (%*ph)\n", __func__, ret, size, data );
	return ret;
}

static void canscreen_SensorsDevice_feature_work ( struct work_struct *work ) {
	struct canscreen_SensorDevice *sensors = container_of ( work, struct canscreen_SensorDevice, feature_work );
	int ret;

	mutex_lock ( &sensors -> mutex );
	if ( !sensors -> connected ) goto out_unlock;

	if ( sensors -> pending_type == hwmon_temp ) {
		ret = canscreen_recv_report ( sensors -> hdev, sensors -> pending_report_id, ( u8* ) &sensors -> temp_features, sizeof ( sensors -> temp_features ) );
		if ( ret > 0 ) sensors -> updated_temp = jiffies;
	}
	else if ( sensors -> pending_type == hwmon_in ) {
		ret = canscreen_recv_report ( sensors -> hdev, sensors -> pending_report_id, ( u8* ) &sensors -> in_features, sizeof ( sensors -> in_features ) );
		if ( ret > 0 ) sensors -> updated_in = jiffies;
	}

out_unlock:
	mutex_unlock ( &sensors -> mutex );
}

static void canscreen_HighFlow2Device_feature_work ( struct work_struct *work ) {
	struct canscreen_HighFlow2Device *highflow2 = container_of ( work, struct canscreen_HighFlow2Device, feature_work );
	int ret;

	mutex_lock ( &highflow2 -> mutex );
	if ( !highflow2 -> connected ) goto out_unlock;

	if ( highflow2 -> pending_type == hwmon_temp ) {
		ret = canscreen_recv_report ( highflow2 -> hdev, highflow2 -> pending_report_id, ( u8* ) &highflow2 -> temp_features, sizeof ( highflow2 -> temp_features ) );
		if ( ret > 0 ) highflow2 -> updated_temp = jiffies;
	}
	else if ( highflow2 -> pending_type == hwmon_fan ) {
		ret = canscreen_recv_report ( highflow2 -> hdev, highflow2 -> pending_report_id, ( u8* ) &highflow2 -> flow_features, sizeof ( highflow2 -> flow_features ) );
		if ( ret > 0 ) highflow2 -> updated_flow = jiffies;
	}

	out_unlock:
	mutex_unlock ( &highflow2 -> mutex );
}

static int canscreen_WakeUp_Collection ( struct hid_device *hdev, u8 report_id, struct canscreen_SensorsFeatures *feature ) {
	int ret;
	struct canscreen_SensorsFeatures *dummy_feature = NULL;
	mutex_lock ( &canscreen_ep0_lock );

	ret = canscreen_recv_report ( hdev, report_id, ( u8* ) feature, sizeof ( *feature ) );
	msleep ( 20 );
	if ( ret < 0 ) {
		hid_warn ( hdev, "Impossible de lire l'état initial de la collection %d (err: %d). Configuration par défaut forcée.\n", report_id, ret );
		memset ( feature, 0, sizeof ( *feature ) );
		feature -> Connection = 0;					// SENSOR_CONNECTION_TYPE_PC_INTEGRATED
		feature -> ReportState = 1;						// SENSOR_REPORTING_STATE_REPORT_ALL_EVENTS
		feature -> Sensivity = cpu_to_le16 ( 10 );
	}
	else hid_dbg ( hdev, "État initial collection %d lu: Interval=%u ms, PowerState=%d, Status=%d\n", report_id, le32_to_cpu ( feature -> Interval ), feature -> PowerState, feature -> Status );

	feature -> PowerState = 1;						// SENSOR_POWER_STATE_D0_FULL_POWER
	feature -> Status = 1;							// SENSOR_STATE_READY
	feature -> Interval = cpu_to_le32 ( 100 );

	ret = canscreen_send_report ( hdev, report_id, ( u8* ) feature, sizeof ( *feature ) );
	msleep ( 20 );
	if ( ret < 0 ) {
		hid_err ( hdev, "Échec de l'envoi du rapport de réveil à la collection %d: %d\n", report_id, ret );
		goto fail_exit;
	}

	dummy_feature = kzalloc ( sizeof ( *dummy_feature ), GFP_KERNEL );
	if ( !dummy_feature ) {
		hid_err ( hdev, "Impossible d'allouer de la mémoire pour le retour de configuration.\n" );
		ret = -ENOMEM;
		goto fail_exit;
	}

	ret = canscreen_recv_report ( hdev, report_id, ( u8* ) dummy_feature, sizeof ( *dummy_feature ) );
	msleep ( 20 );
	if ( ret < 0 ) {
		hid_err ( hdev, "Échec de la lecture de confirmation pour la collection %d: %d\n", report_id, ret );
		goto free_and_exit;
	}

	if ( feature -> PowerState == dummy_feature -> PowerState && feature -> Status == dummy_feature -> Status && feature -> Interval == dummy_feature -> Interval ) {
		hid_info ( hdev, "Collection %d interrogée et réveillée avec succès (Power State D0 Full Power + State Ready)\n", report_id );
	}
	else hid_err ( hdev, "État de la collection %d différent. Interval=%u ms, attendu %u ms, PowerState=%d, attendu %d, Status=%d, attendu %d.\n", report_id, le32_to_cpu ( dummy_feature -> Interval ), le32_to_cpu ( feature -> Interval ), dummy_feature -> PowerState, feature -> PowerState, dummy_feature -> Status, feature -> Status );

free_and_exit:
	kfree ( dummy_feature );
fail_exit:
	mutex_unlock ( &canscreen_ep0_lock );
	return ret;
}
//END



//BEGIN - Le driver HID !
static void canscreen_remove ( struct hid_device *hdev ) {
	struct usb_interface *interface;
	int interface_number;
	void *data = hid_get_drvdata ( hdev );

	if ( !hid_is_usb ( hdev ) ) {
		hid_hw_close ( hdev );
		hid_hw_stop ( hdev );
		return;
	}

	interface = to_usb_interface ( hdev -> dev.parent );
	interface_number = interface -> cur_altsetting -> desc.bInterfaceNumber;

	if ( data ) {
		if ( interface_number == SENSORS_INTERFACE ) {
			struct canscreen_SensorDevice *sensors = data;
			sensors -> connected = false;
			cancel_work_sync ( &sensors -> feature_work );
		}
		else if ( interface_number == HIGHFLOW2_INTERFACE ) {
			struct canscreen_HighFlow2Device *highflow2 = data;
			highflow2 -> connected = false;
			cancel_work_sync ( &highflow2 -> feature_work );
		}
	}

	hid_hw_close ( hdev );
	hid_hw_stop ( hdev );
}

static int canscreen_probe ( struct hid_device *hdev, const struct hid_device_id *id ) {
	int ret;
	struct usb_interface *interface;
	int interface_number;

	ret = hid_parse ( hdev );
	if ( ret ) {
		hid_err ( hdev, "%s: Erreur lors du parsing du descripteur HID\n", __func__ );
		return ret;
	}

	hdev -> quirks |= HID_QUIRK_ALWAYS_POLL;
	ret = hid_hw_start ( hdev, HID_CONNECT_HIDRAW | HID_CONNECT_DRIVER );
	//ret = hid_hw_start ( hdev, HID_CONNECT_HIDRAW );
	if ( ret ) {
		hid_err ( hdev, "%s: Impossible de démarrer le matériel HID\n", __func__ );
		return ret;
	}

	ret = hid_hw_open ( hdev );
	if ( ret ) {
		hid_err ( hdev, "%s: Impossible d'ouvrir le matériel HID\n", __func__ );
		goto err_stop;
	}

	if ( !strstr ( hdev -> name, BOARD_NAME ) ) {
		hid_warn ( hdev, "Périphérique ignoré: Le produit '%s' ne correspond pas à '%s'. \n", hdev -> name ? hdev -> name : "Inconnu", BOARD_NAME );
		ret = -ENODEV;
		goto err_close;
	}

	if ( !hid_is_usb ( hdev ) ) {
		ret = -ENODEV;
		goto err_close;
	}
	interface = to_usb_interface ( hdev -> dev.parent );
	interface_number = interface -> cur_altsetting -> desc.bInterfaceNumber;

	switch ( interface_number ) {
		case SENSORS_INTERFACE:
			{
				struct canscreen_SensorDevice *sensors;

				sensors = devm_kzalloc ( &hdev -> dev, sizeof ( *sensors ), GFP_KERNEL );
				if ( !sensors ) {
					ret = -ENOMEM;
					goto err_close;
				}

				sensors -> hdev = hdev;
				sensors -> connected = true;
				mutex_init ( &sensors -> mutex );
				spin_lock_init ( &sensors -> spinlock );
				INIT_WORK ( &sensors -> feature_work, canscreen_SensorsDevice_feature_work );

				sensors -> temp_label = canscreen_SensorsDevice_temp_labels;
				sensors -> in_label = canscreen_SensorsDevice_in_labels;

				sensors -> temp_input [ 0 ] = -ENODATA;
				sensors -> temp_input [ 1 ] = -ENODATA;

				canscreen_WakeUp_Collection ( hdev, SENSORSDEVICE_TEMPERATURE_ID, &sensors -> temp_features );
				sensors -> updated_temp = jiffies;

				canscreen_WakeUp_Collection ( hdev, SENSORSDEVICE_MONITOR_ID, &sensors -> in_features );
				sensors -> updated_in = jiffies;

				hid_set_drvdata ( hdev, sensors );

				sensors -> hwmon_dev = devm_hwmon_device_register_with_info ( &hdev -> dev, "can_screen_sensors", sensors, &canscreen_SensorsDevice_chip_info, NULL );
				if ( IS_ERR ( sensors -> hwmon_dev ) ) {
					ret = PTR_ERR ( sensors -> hwmon_dev );
					goto err_close;
				}
				hid_info ( hdev, "Interface '%d' [%s] liée avec succès à hwmon\n", interface_number, SENSORSDEVICE_NAME );
			}
			break;
		case HIGHFLOW2_INTERFACE:
			{
				struct canscreen_HighFlow2Device *highflow2;
				
				highflow2 = devm_kzalloc ( &hdev -> dev, sizeof ( *highflow2 ), GFP_KERNEL );
				if ( !highflow2 ) {
					ret = -ENOMEM;
					goto err_close;
				}

				highflow2 -> hdev = hdev;
				highflow2 -> connected = true;
				mutex_init ( &highflow2 -> mutex );
				spin_lock_init ( &highflow2 -> spinlock );
				INIT_WORK ( &highflow2 -> feature_work, canscreen_HighFlow2Device_feature_work );

				highflow2 -> temp_label = canscreen_HighFlow2Device_labels;
				highflow2 -> flow_label = canscreen_HighFlow2Device_labels;

				highflow2 -> temp_input [ 0 ] = -ENODATA;
				highflow2 -> flow_input [ 0 ] = -ENODATA;
				highflow2 -> flow_input [ 1 ] = -ENODATA;

				canscreen_WakeUp_Collection ( hdev, HIGHFLOW2DEVICE_TEMPERATURE_ID, &highflow2 -> temp_features );
				highflow2 -> updated_temp = jiffies;

				canscreen_WakeUp_Collection ( hdev, HIGHFLOW2DEVICE_FLOW_ID, &highflow2 -> flow_features );
				highflow2 -> updated_flow = jiffies;

				hid_set_drvdata ( hdev, highflow2 );

				highflow2 -> hwmon_dev = devm_hwmon_device_register_with_info ( &hdev -> dev, "aquacomputer_highflow_2", highflow2, &canscreen_HighFlow2Device_chip_info, NULL );
				if ( IS_ERR ( highflow2 -> hwmon_dev ) ) {
					ret = PTR_ERR ( highflow2 -> hwmon_dev );
					goto err_close;
				}
				hid_info ( hdev, "Interface '%d' [%s] liée avec succès à hwmon\n", interface_number, HIGHFLOW2DEVICE_NAME );
			}
			break;
		case SCREEN_INTERFACE:
			/*if ( !strstr ( hdev -> name, SCREENDEVICE_NAME ) ) {
				hid_err ( hdev, "Sécurité: L'interface '%d' ne contient pas '%s' (Reçu: '%s')\n", interface_number, SCREENDEVICE_NAME, hdev -> name ? hdev -> name : "Aucun" );
				ret = -ENODEV;
				goto err_close;
			}*/

			hid_info ( hdev, "Interface '%d' [%s] détectée et initialisée\n", interface_number, SCREENDEVICE_NAME );
			break;
		default:
			hid_warn ( hdev, "Interface HID inconnue détectée: %d. Ignorée.\n", interface_number );
			ret = -ENODEV;
			goto err_close;
	}

	return 0;

err_close:
	hid_hw_close ( hdev );
err_stop:
	hid_hw_stop ( hdev );
	return ret;
}

static int canscreen_raw_event ( struct hid_device *hdev, struct hid_report *report, u8 *data, int size ) {
	struct usb_interface *interface;
	unsigned long flags;
	int interface_number;
	u8 report_id = data [ 0 ];
	hid_info_once(hdev, "raw_event reçu: id=%d size=%d\n", data[0], size);
	u8 position, offset;
	//bool dynamic_feature_triggered = false;

	if ( !hid_is_usb ( hdev ) ) return 0;
	interface = to_usb_interface ( hdev -> dev.parent );
	interface_number = interface -> cur_altsetting -> desc.bInterfaceNumber;

	switch ( interface_number ) {
		case SENSORS_INTERFACE:
			{
				struct canscreen_SensorDevice *sensors = hid_get_drvdata ( hdev );
				if ( !sensors ) return 0;

				//mutex_lock ( &sensors -> mutex );
				spin_lock_irqsave ( &sensors -> spinlock, flags );
				if ( report_id == SENSORSDEVICE_TEMPERATURE_ID ) {
					if ( size >= 7 ) {
						// On oublie les deux premiers octets qui correspondent à ReportState et Status.
						s16 raw_chip = ( s16 ) get_unaligned_le16 ( &data [ 3 ] );
						s16 raw_box = ( s16 ) get_unaligned_le16 ( &data [ 5 ] );
						sensors -> temp_input [ 0 ] = hid_raw_to_milli_degree_celsius_convert ( raw_chip );
						sensors -> temp_input [ 1 ] = hid_raw_to_milli_degree_celsius_convert ( raw_box );
						sensors -> updated_temp = jiffies;
					}
				}
				if ( report_id == SENSORSDEVICE_MONITOR_ID ) {
					if ( size >= 19 ) {
						for ( position = 0, offset = 5; position < 7; position ++, offset += 2 ) {
							sensors -> in_input [ position ] = ( s16 ) get_unaligned_le16 ( &data [ offset ] );
						}
						sensors -> updated_in = jiffies;
					}
				}
				//mutex_unlock ( &sensors -> mutex );
				spin_unlock_irqrestore ( &sensors -> spinlock, flags );
				break;
			}
		case HIGHFLOW2_INTERFACE:
			{
				struct canscreen_HighFlow2Device *highflow2 = hid_get_drvdata ( hdev );
				if ( !highflow2 ) return 0;

				//mutex_lock ( &highflow2 -> mutex );
				spin_lock_irqsave ( &highflow2 -> spinlock, flags );
				if ( report_id == HIGHFLOW2DEVICE_TEMPERATURE_ID ) {
					if ( size >= 5 ) {
						s16 raw_temp = ( s16 ) get_unaligned_le16 ( &data [ 3 ] );
						highflow2 -> temp_input [ 0 ] = hid_raw_to_milli_degree_celsius_convert ( raw_temp );
						highflow2 -> updated_temp = jiffies;
					}
				}
				if ( report_id == HIGHFLOW2DEVICE_FLOW_ID ) {
					if ( size >= 5 ) {
						s16 raw_flow = ( s16 ) get_unaligned_le16 ( &data [ 3 ] );
						highflow2 -> flow_input [ 0 ] = hid_raw_to_liter_per_hour_convert ( raw_flow );
						highflow2 -> flow_input [ 1 ] = hid_raw_to_speed_convert ( raw_flow );
						highflow2 -> updated_flow = jiffies;
					}
				}
				//mutex_unlock ( &highflow2 -> mutex );
				spin_unlock_irqrestore ( &highflow2 -> spinlock, flags );
				break;
			}
		//case SCREEN_INTERFACE:
		//	if ( dynamic_feature_triggered ) schedule_work ( &devine3 -> feature_work );
		//	break;
	}

	return 1;
}

/*static __u8 *canscreen_report_fixup ( struct hid_device *hdev, __u8 *rdesc, unsigned int *rsize ) {
	if ( hdev -> vendor == CANSCREEN_VENDOR_ID && hdev -> product == CANSCREEN_PRODUCT_ID ) hdev -> group = CANSCREEN_GROUP_ID;
	return rdesc;
}*/

static bool canscreen_hid_match ( struct hid_device *hdev, bool ignore_special_driver ) {
	if ( hdev -> vendor != CANSCREEN_VENDOR_ID || hdev -> product != CANSCREEN_PRODUCT_ID ) return false;
	//if ( hdev -> name [ 0 ] != '\0' && strcmp ( hdev -> name, BOARD_NAME ) == 0 ) return ignore_special_driver;
	if ( hdev -> name [ 0 ] != '\0' && strcmp ( hdev -> name, BOARD_NAME ) == 0 ) return true;
	return false;
}

static const struct hid_device_id canscreen_id_table [] = {
	{ HID_USB_DEVICE ( CANSCREEN_VENDOR_ID, CANSCREEN_PRODUCT_ID ) },
	{},
};
//END



MODULE_DEVICE_TABLE ( hid, canscreen_id_table );

static struct hid_driver canscreen_driver = {
	.name = DRIVER_NAME,
	.id_table = canscreen_id_table,
	.probe = canscreen_probe,
	.remove = canscreen_remove,
	.raw_event = canscreen_raw_event,
	//.match=canscreen_hid_match,
	//.report_fixup = canscreen_report_fixup,
};

/*static int __init canscreen_init ( void ) {
	int ret = hid_register_driver ( &canscreen_driver );
	if ( ret ) return ret;
	return 0;
}*/
static int __init canscreen_init ( void )										{ return hid_register_driver ( &canscreen_driver ); }
static void __exit canscreen_exit ( void )										{ hid_unregister_driver ( &canscreen_driver ); }

late_initcall ( canscreen_init );
module_exit ( canscreen_exit );

