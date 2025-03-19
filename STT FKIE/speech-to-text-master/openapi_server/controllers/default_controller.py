import connexion
from typing import Dict
from typing import Tuple
from typing import Union

from openapi_server.models.error import Error  # noqa: E501
from openapi_server.models.on_new_text_post_request import OnNewTextPostRequest  # noqa: E501
from openapi_server.models.provide_audio_request import ProvideAudioRequest  # noqa: E501
from openapi_server import util


def initialize(speechtotext_variant, config_profile, webhook_url):  # noqa: E501
    """Initialize the SpeechToText component

    Initialize the SpeechToText component # noqa: E501

    :param speechtotext_variant: 
    :type speechtotext_variant: str
    :param config_profile: 
    :type config_profile: str
    :param webhook_url: 
    :type webhook_url: str

    :rtype: Union[None, Tuple[None, int], Tuple[None, int, Dict[str, str]]
    """
    return 'do some magic!'


def on_new_text_post(body=None):  # noqa: E501
    """on_new_text_post

     # noqa: E501

    :param on_new_text_post_request: Webhook for delivering new text from speech recognition to client
    :type on_new_text_post_request: dict | bytes

    :rtype: Union[None, Tuple[None, int], Tuple[None, int, Dict[str, str]]
    """
    on_new_text_post_request = body
    if connexion.request.is_json:
        on_new_text_post_request = OnNewTextPostRequest.from_dict(connexion.request.get_json())  # noqa: E501
    return 'do some magic!'


def provide_audio(body):  # noqa: E501
    """Provide audio to the SpeechToText component

    Provide audio to the SpeechToText component # noqa: E501

    :param provide_audio_request: part of the audio (e.g. 20 ms) as byte array
    :type provide_audio_request: dict | bytes

    :rtype: Union[None, Tuple[None, int], Tuple[None, int, Dict[str, str]]
    """
    provide_audio_request = body
    if connexion.request.is_json:
        provide_audio_request = ProvideAudioRequest.from_dict(connexion.request.get_json())  # noqa: E501
    return 'do some magic!'
