import unittest

from flask import json

from openapi_server.models.error import Error  # noqa: E501
from openapi_server.models.on_new_audio_post_request import OnNewAudioPostRequest  # noqa: E501
from openapi_server.models.provide_text_request import ProvideTextRequest  # noqa: E501
from openapi_server.test import BaseTestCase


class TestDefaultController(BaseTestCase):
    """DefaultController integration test stubs"""

    def test_initialize(self):
        """Test case for initialize

        Initialize the TextToSpeech component
        """
        query_string = [('texttospeech_variant', 'texttospeech_variant_example'),
                        ('config_profile', 'config_profile_example'),
                        ('webhook_url', 'webhook_url_example')]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/texttospeech/initialize',
            method='PUT',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_on_new_audio_post(self):
        """Test case for on_new_audio_post

        
        """
        on_new_audio_post_request = openapi_server.OnNewAudioPostRequest()
        headers = { 
            'Accept': 'application/json',
            'Content-Type': 'application/json',
        }
        response = self.client.open(
            '/texttospeech/on_new_audio',
            method='POST',
            headers=headers,
            data=json.dumps(on_new_audio_post_request),
            content_type='application/json')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_provide_text(self):
        """Test case for provide_text

        Provide text to the TextToSpeech component
        """
        provide_text_request = openapi_server.ProvideTextRequest()
        headers = { 
            'Accept': 'application/json',
            'Content-Type': 'application/json',
        }
        response = self.client.open(
            '/texttospeech/provide_text',
            method='POST',
            headers=headers,
            data=json.dumps(provide_text_request),
            content_type='application/json')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    unittest.main()
