from flask import Flask, request
import wave
import struct
import speech_recognition as sr
import pyttsx3
import soundfile as sf
from scipy.signal import resample
import numpy as np
from PIL import Image,ImageEnhance
from io import BytesIO
import cv2
import pyautogui
import tkinter as tk
import matplotlib.pyplot as plt
from paddleocr import PaddleOCR, draw_ocr
import matplotlib.patches as patches
import matplotlib.path as mpath
import numexpr
from nltk.corpus import wordnet
from googletrans import Translator





app = Flask(__name__)

client_pen_status=""
audio_play_string="0"
audio_play_string_2="hello"


img_rec_string="-0"

prev_image=None
current_image=None

mouse_x_pos=600
mouse_y_pos=400

graph_x_pos=300
graph_y_pos=300

sensi_mouse=8.0
sensi_graph=2.0

left_click=False
    
canvas=None


@app.route('/upload_pen_status', methods=['POST'])
def upload_pen_statuts():
    global audio_play_string,canvas,left_click,last_x, last_y,last_dot_x,last_dot_y,img_rec_string
    audio_play_string="0"
    img_rec_string="-0"
    client_pen_status=request.data.decode('utf-8')
    print(client_pen_status)
    if(client_pen_status=="audio_sent"):
        create_wav()
        #text_val=wav_to_text(language="hi-IN")
        text_val=wav_to_text("en-IN")
        text_to_speech(text_val)
    if(client_pen_status=="audio_hi_sent"):
        create_wav()
        #text_val=wav_to_text(language="hi-IN")
        text_val=wav_to_text("hi-IN")
        #text_to_speech(text_val)
    if(client_pen_status=="draw"):
        print("drawing window open")
        root = tk.Tk()
        root.title("Point Plotter")
        canvas = tk.Canvas(root, width=800, height=800,bg="white")
        canvas.pack()
        root.mainloop()
    if(client_pen_status=="right_stylus_button_clicked"):
        last_x=None 
        last_y=None
        last_dot_x,last_dot_y=None,None
        if(left_click==True):
            left_click=False
        else:
            left_click=True
    return "", 200

def distance_points(point_1,point_2):
    return ((point_1[0]-point_2[0])*2+(point_1[1]-point_2[1])*2)*(1/2)

def get_meaning(word):
    synsets = wordnet.synsets(word)
    if synsets:
        return synsets[0].definition()
    else:
        return "Meaning not found"

def find_word(image_data,ocr_an):
    image = Image.open(BytesIO(image_data))
    rotated_image = image.rotate(90, expand=True)
    rotated_image.save("img_store/received_image.jpeg")
    #rotated_image = Image.open("img_store/received_image.jpeg")
    width, height = rotated_image.size
    crop_rectangle = (width/6, height/4, width*5/6, height*3/4)  # Replace with your coordinates
    #crop_rectangle = (height/3, width/5, height*4/5, width*3/4)
    cropped_image = rotated_image.crop(crop_rectangle)

    enhancer = ImageEnhance.Brightness(cropped_image)
    bright_image = enhancer.enhance(1.5)
    bright_image.save("img_store/received_image.jpeg")

    image = Image.open("img_store/received_image.jpeg")
    fig, (ax1,ax2,ax3) = plt.subplots(1, 3, figsize=(15, 7.5), gridspec_kw={'width_ratios': [4, 4, 2]})
    ax1.imshow(image)
    ax1.set_axis_off()
    ax2.set_axis_off()
    width, height = image.size
    rect = patches.Rectangle((0, 0), width-1, height-1, linewidth=2, edgecolor='purple', facecolor='none')
    ax2.set_xlim([0, width])
    ax2.set_ylim([height, 0]) 
    ax2.set_aspect('equal')
    ax2.add_patch(rect)

    result = ocr_an.ocr('img_store/received_image.jpeg', cls=True)
    updated_result=result[0]
    dictionary_result={}
    bias=0
    dict_key=0
    for line in updated_result:
        total_x=line[0][0][0]-line[0][2][0]
        total_y=line[0][0][1]-line[0][2][1]
        if(total_x<0):
            total_x=-total_x
        if(total_y<0):
            total_y=-total_y
        total_char=len(line[1][0])
        total_x=total_x+bias
        char_x=total_x/total_char
        total_word=line[1][0].split(' ')
        current_x=line[0][0][0]
        current_y=line[0][0][1]
        for words in total_word:
            word_boundary=[(current_x,current_y),(current_x+len(words)*char_x,current_y),(current_x+len(words)*char_x,current_y+total_y),(current_x,current_y+total_y)]
            dictionary_result[dict_key]=[words,word_boundary]
            #ax2.text(current_x,current_y,words, fontsize=8, color='red', ha='left', va='center',weight='bold')
            dict_key=dict_key+1
            current_x=current_x+len(words)*char_x+char_x
        print(dictionary_result)

    rec_rand=[]

    for number, (word, boundary_box) in dictionary_result.items():
        print(boundary_box)
        rec_rand.append(boundary_box)
        bbox = patches.Polygon(boundary_box,closed=True, linewidth=1, edgecolor='y', facecolor='none')
        ax1.add_patch(bbox)
        ax1.text(boundary_box[0][0], boundary_box[0][1], str(number), color='b', fontsize=10, ha='right', va='top', weight='bold')
        
        rec_k=rec_rand[0][2][1]-rec_rand[0][1][1]
    if(rec_k<0):
        reck_k=-rec_k

    x1=width/2-rec_k/4
    x2=height/2-rec_k/4
    x3=width/2+rec_k/4
    x4=height/2+rec_k/4

    a=[(x1,x2),(x1,x4),(x3,x4),(x3,x2)]
    polygon_a = patches.Polygon(a, closed=True,  linewidth=1 , edgecolor='g', facecolor='g')
    ax2.add_patch(polygon_a)
    polygon_b = patches.Polygon(a, closed=True,  linewidth=1 , edgecolor='g', facecolor='g')
    ax1.add_patch(polygon_b)
    print(a)

    ax3.set_axis_off()
    x_pos_word_ax2=0.1
    y_pos_word_ax2=0.97
    ax3.text(0.5, 1.0, 'Recognized Words With Numbers:', fontsize=15, ha='center', va='center', weight='bold',color='purple')
    for j in dictionary_result.keys():
        if(j==16):
            x_pos_word_ax2=x_pos_word_ax2+0.5
            y_pos_word_ax2=0.97
        ax3.text(x_pos_word_ax2, y_pos_word_ax2 , f"{j} - {dictionary_result[j][0]}", fontsize=14, ha='left', va='top',fontfamily='Nirmala UI')
        y_pos_word_ax2=y_pos_word_ax2-0.04
        ax2.text(dictionary_result[j][1][0][0], dictionary_result[j][1][0][1], str(j), color='b', fontsize=8, ha='right', va='top', weight='bold')
        ax2.text(dictionary_result[j][1][0][0],dictionary_result[j][1][0][1],dictionary_result[j][0], fontsize=15, color='red', ha='left', va='bottom',weight='bold',fontfamily='Nirmala UI')

    plt.tight_layout()
    plt.subplots_adjust(wspace=0.1)
    plt.savefig('img_store/plt_rec.png', bbox_inches='tight', pad_inches=0.1)
    plt.close()

    distance_list=[]
    smallest_val=10000
    smallest_index=-1
    si=0

    tell_k=-1
    path_a = mpath.Path(a)

    for i in rec_rand:
        path_i = mpath.Path(i)
        if path_i.contains_path(path_a):
            tell_k=tell_k+1
            smallest_index=si
        si=si+1
    if(tell_k==-1):
        si=0
        for point in rec_rand:
            avp=0.0
            for x in a:
                avx=0.0
                for each_point in point:
                    avx=avx+distance_points(x,each_point)
                avx=avx/4
                avp=avp+avx
            avp=avp/4
            if(avp<smallest_val):
                smallest_val=avp
                smallest_index=si
            distance_list.append(avp)
            si=si+1
        print(distance_list)
        print(smallest_val)

    print(smallest_index)

    ret=dictionary_result[smallest_index][0]
    print(ret)
    return ret

@app.route('/send_data_client')
def send_data_client():
    global img_rec_string
    #print(img_rec_string)
    return img_rec_string

@app.route('/upload_image_rec', methods=['POST'])
def upload_image_rec():
    global img_rec_string
    image_data = request.data
    try:
        abc_te=find_word(image_data,ocr_en)
        img_rec_string=abc_te+"__"+get_meaning(abc_te)
    except:
        img_rec_string="NA__NA"
    return "Image received rec", 200

@app.route('/upload_image_rec_cal', methods=['POST'])
def upload_image_rec_cal():
    global img_rec_string
    image_data = request.data
    expression_in = find_word(image_data,ocr_en)
    expression=""
    for i in range(0,len(expression_in)):
        if(expression_in[i] == "x" or expression_in[i] == "X"):
            expression=expression+'*'
        elif(expression_in[i] == "÷" ):
            expression=expression+'/'
        else:
            expression=expression+expression_in[i]
    print(expression)
    try:
        result = numexpr.evaluate(expression)
        print(result)  
        result=str(result)
    except Exception as e:
        result="NA"
        print(f"Error translating {expression}: {e}")
    img_rec_string=expression+"__"+result
    return "Image received rec", 200

@app.route('/upload_image_translate_EH', methods=['POST'])
def upload_image_translate_EH():
    image_data = request.data
    find_word(image_data,ocr_en)
    return "Image received rec", 200

@app.route('/upload_image_translate_HE', methods=['POST'])
def upload_image_translate_HE():
    global img_rec_string
    image_data = request.data
    ab=find_word(image_data,ocr_hi)
    try:
        translation = translator.translate(ab, src='hi', dest='en')
        if translation.text is not None:
            print(translation.text)
            img_rec_string=translation.text+"__"+get_meaning(translation.text)
        else:
            print("Translation failed for:", ab)
            img_rec_string="NA__NA"
    except Exception as e:
        img_rec_string="NA__NA"
        print(f"Error translating {ab}: {e}")
    return "Image received rec", 200

def phase_calculate_shift(image1, image2):
    image1_cv = cv2.cvtColor(np.array(image1), cv2.COLOR_RGB2GRAY)
    image2_cv = cv2.cvtColor(np.array(image2), cv2.COLOR_RGB2GRAY)
    if image1_cv.shape != image2_cv.shape:
        raise ValueError("Images must have the same dimensions")
    image1_dft = cv2.dft(np.float32(image1_cv), flags=cv2.DFT_COMPLEX_OUTPUT)
    image2_dft = cv2.dft(np.float32(image2_cv), flags=cv2.DFT_COMPLEX_OUTPUT)
    shift_x, shift_y = cv2.phaseCorrelate(np.fft.fftshift(image1_dft[:, :, 0]), np.fft.fftshift(image2_dft[:, :, 0]))[:2]
    return shift_x, shift_y

def calculate_shift(image1, image2):
    image1_cv = cv2.cvtColor(np.array(image1), cv2.COLOR_RGB2GRAY)
    image2_cv = cv2.cvtColor(np.array(image2), cv2.COLOR_RGB2GRAY)
    if image1_cv.shape != image2_cv.shape:
        raise ValueError("Images must have the same dimensions")
    warp_mode = cv2.MOTION_TRANSLATION
    warp_matrix = np.eye(2, 3, dtype=np.float32)
    criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 5000, 1e-10)
    (cc, warp_matrix) = cv2.findTransformECC(image1_cv, image2_cv, warp_matrix, warp_mode, criteria)
    shift_x = warp_matrix[0, 2]
    shift_y = warp_matrix[1, 2]
    return shift_x, shift_y


@app.route('/upload_image_stylus', methods=['POST'])
def upload_image_stylus():
    global prev_image,current_image,mouse_x_pos,mouse_y_pos,sensi_mouse,left_click
    image_data = request.data
    image = Image.open(BytesIO(image_data))
    rotated_image = image.rotate(90, expand=True)
    prev_image=current_image
    current_image=rotated_image
    rotated_image.save("img_store/received_image.jpeg")
    if prev_image is not None:
        shift_x, shift_y = calculate_shift(prev_image, current_image)
        mouse_x_pos=mouse_x_pos-shift_x*sensi_mouse
        mouse_y_pos=mouse_y_pos-shift_y*sensi_mouse
        pyautogui.moveTo(ema_x(mouse_x_pos,0.6,0.3,0.1),ema_y(mouse_y_pos,0.6,0.3,0.1))
        if(left_click==True):
            pyautogui.click(button='left')
            left_click=False
        return f"Image received stylus. Shift in X: {shift_x} pixels, Shift in Y: {shift_y} pixels", 200
    else:
        return "Image received stylus. No previous image to compare shift.", 200

def draw_point(x, y):
    global last_x, last_y
    canvas.create_oval(x-2, y-2, x+2, y+2, fill="green", outline="green")
    if last_x is not None and last_y is not None:
        canvas.create_line(last_x, last_y, x, y, fill="green", width=4)
    last_x, last_y = x, y


def draw_dot(x, y):
    global last_dot_x, last_dot_y
    canvas.create_oval(x-2, y-2, x+2, y+2, fill="green", outline="green")
    if last_dot_x is not None and last_dot_y is not None:
        canvas.create_oval(last_dot_x-2, last_dot_y-2, last_dot_x+2, last_dot_y+2, fill="white", outline="white")
    last_dot_x, last_dot_y = x, y

prev_prev_val_x=None
prev_val_x=None
def ema_x(data, alpha, beta,gamma):
    global prev_val_x,prev_prev_val_x
    if prev_val_x==None and prev_prev_val_x==None:
        prev_prev_val_x=data
        return data
    elif prev_val_x==None and prev_prev_val_x!=None:
        prev_val_x=data
        return data
    else:
        ema_value=alpha*data+beta*prev_val_x+gamma*prev_prev_val_x
        prev_prev_val_x=prev_val_x
        prev_val_x=data
        return ema_value
    
prev_prev_val_y=None
prev_val_y=None
def ema_y(data, alpha, beta,gamma):
    global prev_val_y,prev_prev_val_y
    if prev_val_y==None and prev_prev_val_y==None:
        prev_prev_val_y=data
        return data
    elif prev_val_y==None and prev_prev_val_y!=None:
        prev_val_y=data
        return data
    else:
        ema_value=alpha*data+beta*prev_val_y+gamma*prev_prev_val_y
        prev_prev_val_y=prev_val_y
        prev_val_y=data
        return ema_value


@app.route('/upload_image_draw', methods=['POST'])
def upload_image_draw():
    global prev_image,current_image,graph_x_pos,graph_y_pos,sensi_graph,left_click
    image_data = request.data
    image = Image.open(BytesIO(image_data))
    rotated_image = image.rotate(90, expand=True)
    prev_image=current_image
    current_image=rotated_image
    rotated_image.save("img_store/received_image.jpeg")
    if prev_image is not None:
        shift_x, shift_y = calculate_shift(prev_image, current_image)
        graph_x_pos=graph_x_pos-shift_x*sensi_graph
        graph_y_pos=graph_y_pos-shift_y*sensi_graph
        graph_x_pos_1=ema_x(graph_x_pos,0.6,0.3,0.1)
        graph_y_pos_1=ema_y(graph_y_pos,0.6,0.3,0.1)
        if(left_click==False):
            draw_point(graph_x_pos_1,graph_y_pos_1)
        else:
            draw_dot(graph_x_pos_1,graph_y_pos_1)
        return f"Image received stylus. Shift in X: {shift_x} pixels, Shift in Y: {shift_y} pixels", 200
    else:
        return "Image received stylus. No previous image to compare shift.", 200


@app.route('/upload_audio', methods=['POST'])
def upload_text():
    global audio_play_string
    audio_play_string="0"
    text_data = request.data.decode('utf-8') 
    #print(text_data)
    with open("audio_store/audio_data.txt", "a") as f: 
        f.write(text_data + '\n')  
    return "audio data received", 200



@app.route('/upload_gyro', methods=['POST'])
def upload_gyro():
    text_data = request.data.decode('utf-8') 
    with open("gyro_store/gyro_data.txt", "a") as f:  
        f.write(text_data + '\n')  
    #return "gyro data received", 200
    return "", 200

@app.route('/send_audio')
def send_audio():
    global audio_play_string
    print(audio_play_string)
    return  audio_play_string


def create_wav():
    list=[]
    with open('audio_store/audio_data.txt', 'r') as file:
        data = file.readlines()
        for i in data:
            m=i.strip("/n").split(",")
            list.append(m)
    k=[]
    for i in list:
        for j in i:
            if(j != "" and j!=" " and j!="/n" ):
                k.append(int(j))
    sample_rate = 10000  
    num_samples = 40000  
    audio_samples = k.copy() 
    audio_samples = [max(min(sample, 32767), -32768) for sample in audio_samples]
    audio_data = b''.join(struct.pack('<h', sample) for sample in audio_samples)  # '<h' means little-endian 16-bit
    with wave.open('audio_store/list_to_audio.wav', 'w') as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(2)  # 2 bytes per sample (16-bit)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(audio_data)
    print("WAV file has been created.")
    open("audio_store/audio_data.txt", "w")

def wav_to_text(language): # string output
    global img_rec_string
    wav_file_path="audio_store/list_to_audio.wav"
    recognizer = sr.Recognizer()
    try:
        with sr.AudioFile(wav_file_path) as source:
            print(f"Loading audio file: {wav_file_path}")
            audio_data = recognizer.record(source)
            print("Audio file loaded. Recognizing...")
            text_1 = recognizer.recognize_google(audio_data,language=language)
            print(text_1)
            print("Recognition complete.")
            if(language =="hi-IN"):
                translation = translator.translate(text_1, src='hi', dest='en')
                if translation.text is not None:
                    print(translation.text)
                    img_rec_string=translation.text+"__"+get_meaning(translation.text)
                else:
                    print("Translation failed for:", text_1)
                    img_rec_string="NA__NA"
                print(img_rec_string)
            else:
                img_rec_string="Audio__"+text_1
                print(img_rec_string)                
            return text_1
    except sr.UnknownValueError:
        img_rec_string="Audio__"+"Could not understand the audio."
        return "Could not understand the audio."
    except sr.RequestError as e:
        img_rec_string="Audio__NA"
        return f"Could not request results; {e}"
    except FileNotFoundError:
        img_rec_string="Audio__NA"
        return "File not found. Please check the file path."
    except Exception as e:
        img_rec_string="Audio__NA"
        return f"An unexpected error occurred: {e}"


def text_to_speech(text):
    global audio_play_string
    output_file="audio_store/text_to_audio.wav"
    engine = pyttsx3.init()
    engine.setProperty('rate', 175)  # Speed of speech
    voices = engine.getProperty('voices')
    engine.setProperty('voice', voices[0].id)
    engine.save_to_file(text, output_file)
    engine.runAndWait()

    original_audio, original_sample_rate = sf.read('audio_store/text_to_audio.wav')
    new_sample_rate = 10000 
    resampled_audio = resample(original_audio, int(len(original_audio) * (new_sample_rate / original_sample_rate)))
    resampled_audio = (resampled_audio / np.max(np.abs(resampled_audio)))*5.0
    print("resampled-"+str(len(resampled_audio)))
    #resampled_audio_16bit = np.int16(resampled_audio * 32767)
    resampled_audio_16bit = np.int16(resampled_audio * 127)
    resampled_audio_list = resampled_audio_16bit.tolist()
    sf.write('audio_store/resampled.wav', resampled_audio_16bit, new_sample_rate)
    print("16it-"+str(len(resampled_audio_list)))
    audio_play_string=""
    for i in resampled_audio_list:
        audio_play_string=audio_play_string+","+str(i)
    audio_play_string=str(len(resampled_audio_list))+audio_play_string+","



if __name__ == '__main__':

    last_x, last_y = None, None
    last_dot_x,last_dot_y=None,None
    
    ocr_en = PaddleOCR(use_angle_cls=True, lang='en')
    ocr_hi = PaddleOCR(use_angle_cls=True, lang='hi')


    translator = Translator()

    app.run(host='0.0.0.0', port=7700)