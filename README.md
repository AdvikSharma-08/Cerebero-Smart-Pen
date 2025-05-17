# Cerebro Smart Pen

## Overview

Cerebro is an innovative smart pen developed at the ECE Lab, IIIT Delhi. This advanced portable device integrates scanning and translation features with motion tracking capabilities, significantly improving workflow efficiency and educational engagement.

## Features

### Text Recognition & Translation

- **Real-time scanning** of printed text
- **Multi-language translation** (English-Hindi bidirectional support)
- **Word meaning display** for educational enhancement
- Reduces manual entry time by 2 seconds with 90% translation accuracy

### Mathematical Expression Recognition

- **Expression scanning and computation**
- Supports basic arithmetic operations (+, -, ×, ÷)
- Processes expressions and displays computed results

### Voice Recognition & Audio Feedback

- **Speech-to-text conversion** with language detection
- **Text-to-speech** functionality for auditory feedback
- Supports multiple languages including English and Hindi

### Motion Tracking & Drawing

- **Camera shift detection** transforms the device into a motion pen
- **Drawing functionality** on digital canvas
- Mouse control capabilities through pen movement
- Click functionality through stylus button

## Technical Architecture

### Hardware Components

- ESP32-CAM for image capture and transmission
- Microphone for audio input
- Speaker for audio output
- Stylus button for interaction
- LED illuminator for better image quality in low light

### Software Components

1. **Server (Python/Flask)**

   - Handles incoming data from the smart pen
   - Processes images, audio, and motion data
   - Implements OCR using PaddleOCR
   - Performs translations using Google Translate
   - Provides mathematical expression evaluation
   - Manages drawing and cursor control functionality

2. **Client (ESP32)**
   - Captures images through the onboard camera
   - Records audio through microphone
   - Transmits data to the server
   - Receives processed information back
   - Controls LED illumination
   - Handles user input through stylus buttons

## Implementation Details

### Image Processing Pipeline

1. Image capture from ESP32-CAM
2. Image rotation and preprocessing
3. OCR text extraction using PaddleOCR
4. Text translation using Google Translate
5. Word meaning retrieval from WordNet
6. Mathematical expression evaluation using numexpr

### Motion Tracking Algorithm

- Utilizes phase correlation and ECC (Enhanced Correlation Coefficient) for movement detection
- Implements smoothing with exponential moving average (EMA)
- Provides sensitivity adjustment for different use cases

### Audio Processing Pipeline

1. Audio capture and streaming
2. WAV file creation and manipulation
3. Speech recognition using Google's speech recognition API
4. Text-to-speech conversion using pyttsx3
5. Audio resampling and optimization for playback

## Usage Modes

### Text Recognition Mode

Scan printed text to digitize content, with options for:

- Direct OCR text extraction
- Translation between languages
- Word meaning lookup

### Calculator Mode

Scan mathematical expressions for instant computation.

### Voice Assistant Mode

Speak into the device for:

- Speech-to-text conversion
- Translation of spoken content
- Text-to-speech playback

### Motion Control Mode

Use the pen as a mouse/pointer:

- Control cursor position through pen movement
- Draw on digital canvas
- Click using the stylus button

## User Impact

- Positive feedback from 95% of users for enhanced task management
- 30% increase in user engagement with educational content
- 40% increase in user engagement scores during testing of the motion pen feature

## Future Development

- Expanding language support beyond English and Hindi
- Enhancing mathematical expression recognition for complex equations
- Improving motion tracking precision for detailed drawings
- Adding cloud synchronization for saved content
- Implementing AI-based learning for personalized educational content

## Contributors

- ECE Lab, IIIT Delhi
- UG Researcher Team, April 2024
