#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include <avr/wdt.h> //Feature fehlt, ist aber machbar...

#define TON 128 //Alarm-Sound
#define TRIGGER_ADRESS 0x0F
#define LDR_TRIGGER 128

/*PB0 = Sound (PWM)*/
/*PB2 = Feuchtigkeitssensor*/
/*PB1 = Taster*/
/*PB4 = LDR*/
/*PB3 = Debug-LED*/

volatile uint8_t Boden = 0;
volatile uint8_t Umgebung = 0;

inline void sound_init() {
    TCCR0A |= (1 << WGM01) | (1 << WGM00); //Fast-PWM
    OCR0A = TON;
}

inline void sound_on() {
    TCCR0A |= (1 << COM0A1);
    TCCR0B |= (1 << CS01);
}

void sound_off() {
    TCCR0B = 0;
    TCCR0A &= ~(1 << COM0A1); //Port freigeben
}

inline void adc_init() {
    ACSR |= (1 << ACD); //Komparator ausschalten
    ADMUX |= (1 << ADLAR) | (1 << MUX0);
    ADCSRA |= (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 |= (1 << ADC1D) | (1 << ADC2D);
}

inline void analogRead() {
    ADCSRA |= (1 << ADEN); //Anschalten
}

inline void wdt_init() {
    WDTCR |= (1 << WDTIE) | (1 << WDP3); //4 Sekunden
}

inline void eeprom_init() {
    EECR &= ~((1 << EEPM1) | (1 << EEPM0)); //Atomic
}

inline uint8_t eeprom_read() {
    while(EECR & (1 << EEPE)); //Warte auf Freigabe
    EEARL = TRIGGER_ADRESS;
    EECR |= (1 << EERE);
    return EEDR;
}

inline void eeprom_write() {
    while(EECR & (1 << EEPE)); //Warte auf Freigabe
    EEARL = TRIGGER_ADRESS;
    EEDR = Boden;
    EECR|= (1 << EEMPE);
    EECR |= (1 << EEPE);
}

inline void input_init() {
    PCMSK |= (1 << PCINT1);
    GIMSK |= (1 << PCIE);
}

ISR(ADC_vect) {
    if(ADMUX & (1 << MUX0)) { //Wenn Kanal 1
        ADMUX &= ~(1 << MUX0); //Dann Kanal 2
        ADMUX |= (1 << MUX1);
        Boden = ADCH;
    } else {
        ADCSRA &= ~(1 << ADEN);
        ADMUX &= ~(1 << MUX1); //Kanal 1 und ausschalten
        ADMUX |= (1 << MUX0);
        Umgebung = ADCH;
        if(MCUCR & 0x18) { //Wenn Sleepmode =! IDLE
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        }
    }
}

ISR(WDT_vect) {
    PINB |= (1 << PB3); //Debug
    if((eeprom_read() >= Boden) && Umgebung <= LDR_TRIGGER) { //Wenn Erde zu Trocken und Tag
        if(TCCR0B == 0) { //Wenn Sound ausgeschalten
            sound_on();
            set_sleep_mode(SLEEP_MODE_IDLE);
        } else {
            sound_off();
            set_sleep_mode(SLEEP_MODE_ADC);
        }
    } else {
        sound_off();
        set_sleep_mode(SLEEP_MODE_ADC);
    }
    analogRead(); //Messungen anstoßen
}

ISR(PCINT0_vect) {
    if(!(PINB & (1 << PB1))) {
        eeprom_write();
    }
}

int main(void) {
    DDRB |= (1 << PB0) | (1 << PB3);
    sound_init();
    adc_init();
    eeprom_init();
    input_init();
    wdt_init();
    set_sleep_mode(SLEEP_MODE_ADC);
    sei();
    analogRead();
    while(1) {
        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }
}
