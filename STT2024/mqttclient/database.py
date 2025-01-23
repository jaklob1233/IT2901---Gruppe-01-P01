"""database"""

import logging
import sqlite3
from sqlite3 import Error


class Database:
    """
    Database
    """

    def __init__(self):
        """
        Creates a new Database object

        Parameters:
            None

        Returns:
            None
        """
        # Create a connection to the SQLite database
        conn = self.create_connection()
        if conn is not None:
            # Create the necessary tables in the SQLite database
            self.create_tables(conn)
            conn.close()
        else:
            logging.error("Error creating connection to database")

    def create_connection(self) -> sqlite3.Connection | None:
        """
        Creates a connection to the SQLite database

        Paramters:
            None

        Returns:
            conn (sqlite3.Connection):
            Connection to the SQLite database or None if there is an error
        """
        try:
            conn = sqlite3.connect('ChatLog.db')
            return conn
        except Error as e:
            logging.error(e)
            return None

    def create_tables(self, conn) -> None:
        """
        Creates the necessary tables in the SQLite database

        Parameters:
            conn (sqlite3.Connection): Connection to the SQLite database

        Returns:
            None
        """
        try:
            cursor = conn.cursor()
            # Create Topic table
            cursor.execute('''
            CREATE TABLE IF NOT EXISTS Topic (
                topic TEXT PRIMARY KEY,
                subscribed BOOLEAN NOT NULL
            )
        ''')
            print("Topic table created successfully")

            # Create Message table
            cursor.execute('''
            CREATE TABLE IF NOT EXISTS Message (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                username TEXT NOT NULL,
                message TEXT NOT NULL
            )
        ''')
            print("Message table created successfully")

            # Create MessageTopicRelation table
            cursor.execute('''
            CREATE TABLE IF NOT EXISTS MessageTopicRelation (
                messageID INTEGER,
                topic TEXT,
                FOREIGN KEY (messageID) REFERENCES Message(id),
                FOREIGN KEY (topic) REFERENCES Topic(topic),
                UNIQUE(messageID, topic)
            )
        ''')
            print("MessageTopicRelation table created successfully")

            conn.commit()
        except Error as e:
            logging.error(e)

    def insert_message(self, topic, timestamp, username, message) -> None:
        """
        Inserts a message into the SQLite database

        Parameters:
            topic (str): The topic of the message
            timestamp (str): The timestamp of the message
            username (str): The username of the sender
            message (str): The message text

        Returns:
            None
        """
        conn = self.create_connection()
        if conn is not None:
            try:
                cursor = conn.cursor()
                cursor.execute(
                    "INSERT INTO Message (timestamp, username, message) VALUES (?, ?, ?)",
                    (timestamp,
                     username,
                     message))
                conn.commit()
                message_id = cursor.lastrowid
                cursor.execute(
                    "INSERT INTO MessageTopicRelation (messageID, topic) VALUES (?, ?)",
                    (message_id,
                     topic))
                conn.commit()
                conn.close()
            except Error as e:
                logging.error(e)

    def insert_topic(self, topic) -> None:
        """
        Inserts a topic into the SQLite database

        Parameters:
            topic (str): The topic to be inserted in the database

        Returns:
            None
        """
        conn = self.create_connection()
        if conn is not None:
            try:
                cursor = conn.cursor()
                cursor.execute(
                    "INSERT OR IGNORE INTO Topic (topic, subscribed) VALUES (?, ?)", (topic, 0))
                conn.commit()
                conn.close()
            except Error as e:
                logging.error(e)

    def get_chatlog(self, topic) -> list | None:
        """
        Retrieves the last 10 messages from the chat log for a given topic

        Parameters:
            topic (str): The topic to retrieve the chat log for

        Returns:
            rows (list): List of tuples containing the timestamp, username, and message text
            for the last 10 messages in the chat log
        """
        conn = self.create_connection()
        if conn is not None:
            try:
                cursor = conn.cursor()
                cursor.execute('''
                    SELECT timestamp, username, message
                    FROM Message
                    JOIN MessageTopicRelation ON Message.id = MessageTopicRelation.messageID
                    WHERE topic = ?
                    ORDER BY timestamp DESC
                    LIMIT 10
                ''', (topic,))
                rows = cursor.fetchall()
                conn.close()
                return rows[::-1]
            except Error as e:
                logging.error(e)
                return None
        return None

    def get_subscribed_topics(self) -> list | None:
        """
        Retrieve all topics from database that has subscribed field set to true

        Parameters:
            None

        Returns:
            subscribed_topics (list): List of strings that correspond to topics or None of connections to database fails
        """
        conn = self.create_connection()
        subscribed_topics = []
        if conn is not None:
            try:
                cursor = conn.cursor()
                cursor.execute(
                    "SELECT topic FROM Topic WHERE subscribed = ?", (1,))
                rows = cursor.fetchall()
                for row in rows:
                    subscribed_topics.append(row[0])
                conn.close()
                return subscribed_topics
            except Error as e:
                logging.error(e)
                return None

    def set_subscribed(self, topic, subscribed) -> None:
        """
        Sets subsribed field in topic table based on subscribed parameter

        Parameters:
            topic (str): Topic to update
            subscribed (bool): Value to set in subscribed field

        Returns:
            None
        """
        conn = self.create_connection()
        if conn is not None:
            try:
                cursor = conn.cursor()
                cursor.execute('''
                    UPDATE Topic
                    SET subscribed = ?
                    WHERE topic = ?
                    ''', (subscribed, topic))
                conn.commit()
                conn.close()
            except Error as e:
                logging.error(e)
