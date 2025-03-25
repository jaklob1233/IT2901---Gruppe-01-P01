import unittest

from flask import json

from openapi_server.models.error import Error  # noqa: E501
from openapi_server.models.on_new_text_post_request import OnNewTextPostRequest  # noqa: E501
from openapi_server.models.provide_audio_request import ProvideAudioRequest  # noqa: E501
from openapi_server.test import BaseTestCase


class TestDefaultController(BaseTestCase):
    """DefaultController integration test stubs"""

    def test_initialize(self):
        """Test case for initialize

        Initialize the SpeechToText component
        """
        query_string = [('speechtotext_variant', 'speechtotext_variant_example'),
                        ('config_profile', 'config_profile_example'),
                        ('webhook_url', 'webhook_url_example')]
        headers = { 
            'Accept': 'application/json',
        }
        response = self.client.open(
            '/speechtotext/initialize',
            method='PUT',
            headers=headers,
            query_string=query_string)
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_on_new_text_post(self):
        """Test case for on_new_text_post

        
        """
        on_new_text_post_request = openapi_server.OnNewTextPostRequest()
        headers = { 
            'Accept': 'application/json',
            'Content-Type': 'application/json',
        }
        response = self.client.open(
            '/speechtotext/on_new_text',
            method='POST',
            headers=headers,
            data=json.dumps(on_new_text_post_request),
            content_type='application/json')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))

    def test_provide_audio(self):
        """Test case for provide_audio

        Provide audio to the SpeechToText component
        """
        provide_audio_request = openapi_server.ProvideAudioRequest()
        headers = { 
            'Accept': 'application/json',
            'Content-Type': 'application/json',
        }
        response = self.client.open(
            '/speechtotext/provide_audio',
            method='POST',
            headers=headers,
            data=json.dumps(provide_audio_request),
            content_type='application/json')
        self.assert200(response,
                       'Response body is : ' + response.data.decode('utf-8'))


if __name__ == '__main__':
    unittest.main()
