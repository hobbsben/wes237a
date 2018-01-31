#include "mbed.h"
#include "Adafruit_SSD1306.h"

// an I2C sub-class that provides a constructed default
class I2CPreInit : public I2C
{
public:
    I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl) {
        frequency(400000);
        start();
    };
};

I2CPreInit gI2C(p9,p10);
Adafruit_SSD1306_I2c gOled2(gI2C,p27,0x7a,64,128);

Ticker wordprint;
char myarray[] = "It was love at first sight.\nThe first time Yossarian saw the chaplain he fell madly in love with him.\nYossarian was in the hospital with a pain in his liver that fell just short of being jaundice. The doctors were puzzled by the fact that it wasn\'t quite jaundice. If it became jaundice they could treat it. If it didn\'t become jaundice and went away they could discharge him. But this just being short of jaundice all the time confused them.\nEach morning they came around, three brisk and serious men with efficient mouths and inefficient eyes, accompanied by brisk and serious Nurse Duckett, one of the ward nurses who didn\'t like Yossarian. They read the chart at the foot of the bed and asked impatiently about the pain. They seemed irritated when he told them it was exactly the same.\n\'Still no movement?\' the full colonel demanded.\nThe doctors exchanged a look when he shook his head.\n\'Give him another pill.\'\nNurse Duckett made a note to give Yossarian another pill, and the four of them moved along to the next bed. None of the nurses liked Yossarian. Actually, the pain in his liver had gone away, but Yossarian didn\'t say anything and the doctors never suspected. They just suspected that he had been moving his bowels and not telling anyone.\nYossarian had everything he wanted in the hospital. The food wasn\'t too bad, and his meals were brought to him in bed. There were extra rations of fresh meat, and during the hot part of the afternoon he and the others were served chilled fruit juice or chilled chocolate milk. Apart from the doctors and the nurses, no one ever disturbed him. For a little while in the morning he had to censor letters, but he was free after that to spend the rest of each day lying around idly with a clear conscience. He was comfortable in the hospital, and it was easy to stay on because he always ran a temperature of 101. He was even more comfortable than Dunbar, who had to keep falling down on his face in order to get his meals brought to him in bed.\nAfter he had made up his mind to spend the rest of the war in the hospital, Yossarian wrote letters to everyone he knew saying that he was in the hospital but never mentioning why. One day he had a better idea. To everyone he knew he wrote that he was going on a very dangerous mission. \'They asked for volunteers. It\'s very dangerous, but someone has to do it.\nI\'ll write you the instant I get back.\' And he had not written anyone since.\nAll the officer patients in the ward were forced to censor letters written by all the enlisted-men patients, who were kept in residence in wards of their own. It was a monotonous job, and Yossarian was disappointed to learn that the lives of enlisted men were only slightly more interesting than the lives of officers. After the first day he had no curiosity at all. To break the monotony he invented games. Death to all modifiers, he declared one day, and out of every letter that passed through his hands went every adverb and every adjective. The next day he made war on articles. He reached a much higher plane of creativity the following day when he blacked out everything in the letters but *a, an* and *the*. That erected more dynamic intralinear tensions, he felt, and in just about every case left a message far more universal. Soon he was proscribing parts of salutations and signatures and leaving the text untouched. One time he blacked out all but the salutation \'Dear Mary\' from a letter, and at the bottom he wrote, \'I yearn for you tragically. R. O. Shipman, Chaplain, U.S. Army.\' R.O. Shipman was the group chaplain\'s name.";
bool toprint = false;

void printtime()
{
    toprint =  true;
}

int main()
{
    wait(2);
    gOled2.oled_clear();
    wordprint.attach(&printtime, 1);

    int i,j,ctr;
    char newString[650][15];

    j=0;
    ctr=0;
    for(i=0; i<=(strlen(myarray)); i++) {
        if(myarray[i]==' '||myarray[i]=='\0'||myarray[i]=='\n') {
            newString[ctr][j]='\0';
            ctr++;  //for next word
            j=0;    //for next word, init index to 0
        } else {
            newString[ctr][j]=myarray[i];
            j++;
        }
    }
    
    int count = 0;

    while(1) {
        if (toprint){
            gOled2.printf(newString[count]);
            gOled2.printf(" ");
            gOled2.display();
            count++;
            toprint = false;
        }
    }
}