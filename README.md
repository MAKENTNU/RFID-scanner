# RFID-scanner

Arduino sketch for getting the EM-number from NTNU access cards and sending it to MAKE NTNU server. Used to check in to the makerspace.

The NTNU access cards have two different chips in them. A 13.56 MHz Mifare Classic 1K and a 125 KHz chip. From these chips one can read out the M- and EM-numbers respectively. This sketch finds the EM number.

A UART enabled 125 KHz rfid reader bought at [Omega Verksted](https://omegav.no) is used to scan the cards. It uses a baud rate of 9600.

The card emits 10 bytes when read. These are on the format

| Start | Length | Type   | ID          | Parity | Stop |
|-------|--------|--------|-------------|--------|------|
| 0x02  | 0x0A   | 0x0201 | 0xXXXX XXXX | 0xXX   | 0x03 |

If each byte in the the 4 byte ID is reversed and converted to decimal you get the EM-number printed at the bottom of the backside of your card. All new EM-numbers are ten digits long, however the numeric value may only be nine digits. The old card numbers are seven or eight digits long but they follow the same byte reversal pattern. For consitency and to keep the numeric value, each number is zero-padded to ten digits before being sent to the server.

The EM-number itself does not seem to have any check sums and are likely just incremented for each card issued.

## Settings
#### Host
The full url to send requests to. Must start with "https://" and have a trailing slash.
#### Secret
The key used for authentication with the server. Must be url-encoded.
#### Scanner ID
A unique id that may be used by the server for arbitration between scanners.
