from datetime import datetime

import requests

import json
import traceback

import logging
logger = logging.getLogger(__name__)


def get_time_string(date_time):
    return date_time.strftime("%Y-%m-%dT%H:%M:%S.%f%z")

##

def get_jsonstr_info(client_id, client_name, client_type):
    return json.dumps({
      "COUNTRY": "PRT",
      "ORGANISATION": "PARTICLE",
      "node_id": client_id,
      "assetType": client_type,
      "assetName": client_name,
      "functions": [
        "info",
        "location",
        "vitals"
      ],
      "symbolCode": "EFOPAE----"
    })


def publish_info(mqtt_client, topic_prefix, json_data, qos: int = 0, retain: bool = False):
    topic = topic_prefix + "/info"
    (rc, mid) = mqtt_client.publish(topic=topic, payload=json_data, qos=qos, retain=retain)


def get_jsonstr_location(client_id, msg_id, lon, lat, alt):
    return json.dumps({
        "type": "Feature",
        "geometry": {
          "type": "Point",
          "coordinates": [lon, lat, alt]
        },
        "properties": {
          "node_id": client_id,
          "msg_id": msg_id,
          "timestamp": datetime.now().timestamp()
        }
      })

def publish_location(mqtt_client, topic_prefix, json_data, qos: int = 0, retain: bool = False):
    topic = topic_prefix + "/location"
    (rc, mid) = mqtt_client.publish(topic=topic, payload=json_data, qos=qos, retain=retain)


##


def post_data(endpoint, token, data):
    headers = { "Content-Type": "application/json;charset=utf-8" }
    if token:
        headers = {
            "Content-Type": "application/json;charset=utf-8",
            "Authorization": "Token "+token
        }
    try:
        logger.info(f"-- post to {endpoint}")
        logger.info(f"-- data is {data}")
        #print("-- data is ", data)
        myResponse = requests.post(url=endpoint, json=data, headers=headers, timeout=15)
        if (myResponse.ok):
            logger.info("-- data posted OK.")
            #print("-- data posted OK. Received: ", myResponse.json())
            return 200, None
        else:
            myResponse.raise_for_status()
    except Exception as e:
        logger.error(f"Exception occurred: {e}")
        logger.error(f"Traceback: {traceback.format_exc()}")
        logger.info(f"-- error in post {endpoint}")
    return 400, None
