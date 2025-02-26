# coding: utf-8

"""
    SpeechToText

    This is an API for a Speech-to-Text component.

    The version of the OpenAPI document: 0.1.0
    Contact: norman.jansen@fkie.fraunhofer.de
"""

import requests
from typing import Dict, Optional, Tuple, Union
import logging

logger = logging.getLogger(__name__)

class SpeechToTextApiClient:
    def __init__(self, base_url: str, default_headers: Optional[Dict[str, str]] = None):
        """Initialize the SpeechToText API client.

        Parameters:
            base_url: The base URL of the API.
            default_headers: Optional default headers for all requests.
        """
        self.base_url = base_url
        self.default_headers = default_headers or {}

    def on_new_text_post(
        self,
        on_new_text_post_request: Dict[str, Union[int, str]],
        endpoint: str = "/on_new_text",
        _request_timeout: Union[None, float, Tuple[float, float]] = None,
        _content_type: Optional[str] = "application/json",
        _headers: Optional[Dict[str, str]] = None
    ) -> None:
        """Custom implementation for posting new text.

        Parameters:
            on_new_text_post_request: The JSON payload containing text data and timestamps.
            endpoint: The API endpoint to send the POST request to.
            _request_timeout: Timeout for the HTTP request.
            _content_type: Content-Type for the request, default is application/json.
            _headers: Optional headers for the request.
        """
        url = f"{self.base_url}{endpoint}"
        headers = {**self.default_headers, **(_headers or {})}
        headers["Content-Type"] = _content_type

        # Send the POST request
        try:
            logger.debug("Sending POST request.")
            logger.debug("URL: %s", url)
            logger.debug("Headers: %s", headers)
            logger.debug("Payload: %s", on_new_text_post_request)

            response = requests.post(
                url,
                json=on_new_text_post_request,
                headers=headers,
                timeout=_request_timeout
            )

            # Check the response status
            response.raise_for_status()
            logger.info("Request successful. %s", response.text)

        except requests.exceptions.RequestException as e:
            logger.error("Request failed: %s", str(e))
            logger.debug("Error details", exc_info=True)  # Log traceback for debugging
            raise

