// Arkanoid
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdbool.h>
#include <list>
using namespace std;

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;
 
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;

int blocos = 0;

class Block {
public:
    int comp;
    int x;
    int y;
    bool ativo;
    ALLEGRO_COLOR cor;
    
    Block( int comp, int x, int y, ALLEGRO_COLOR cor ) :
        comp( comp ), x(x), y(y), ativo( true ), cor( cor ){}
    
    void draw() {
        if( ativo )
            al_draw_filled_rectangle(x, y, x + comp, y - comp, cor);
    }
    
    void print() {
        cout << "X = " << x << endl;
        cout << "Y = " << y << endl;
        cout << "COMP = " << comp << endl;
    }
};

class Tabua {
public:
    int comp;
    int altura;
    int x; 
    int vel;
    
    Tabua( int comp, int altura, int x, int vel ) :
        comp( comp ), altura( altura ), x( x ), vel( vel ){}
    
    void draw( ) {
        al_draw_filled_rectangle(x, ALTURA_TELA, x + comp, ALTURA_TELA - altura, al_map_rgb(0, 255, 255));    
    }   
    
   
    
    void left() {
        if( x >= 0 )
            x = x - vel;
        if( x < 0 )
            x = 0; 
    }
    
    void right() {
        if( x + comp <= LARGURA_TELA )
            x = x + vel;
        if( x + comp >=  LARGURA_TELA )
            x = LARGURA_TELA - comp;
    }
};

class Bola {
public:
    int raio;
    int x;
    int y;
    int vel_x;
    int vel_y;
    ALLEGRO_COLOR cor;
    
    Bola( int raio, int x, int y, int vel_x, int vel_y, ALLEGRO_COLOR cor ) :       
    raio( raio ), x(x), y(y), vel_x(vel_x), vel_y(vel_y), cor(cor) {} 
    void draw() {
        al_draw_filled_circle(x, y, raio, cor);
    }
    void print() {
        cout << "X = " << x << endl;
        cout << "Y = " << y << endl;
        cout << "RAIO = " << raio << endl;
    }
    void move(Tabua b) {
        
        x = x + vel_x;
        y = y - vel_y;
        
        
        if( x + raio >= LARGURA_TELA || x - raio <= 0 )
            vel_x = -vel_x;
       
        if( y - raio <= 0 )
            vel_y = -vel_y;   
            
        if( y + raio >= ALTURA_TELA - b.altura ) 
            if( x >= b.x && x <= (b.x + b.comp) )
                vel_y =-vel_y;
    }
};
void colision_detect( Bola & bola, Block & block ) {
    if( block.ativo ) { 
        
        // Colisao de baixo
        if( bola.y - bola.raio <= block.y &&
            bola.y - bola.raio >= block.y - block.comp && 
            bola.x >= block.x && 
            bola.x <= block.x + block.comp ) {
                bola.vel_y = -bola.vel_y;
                block.ativo = false;
            cout << "BAIXO" << endl;
            cout << "New vel = " << bola.vel_y << endl;
                    blocos++;
            return;
        
        }
        // Colisao de cima
        if( bola.y + bola.raio >= block.y - block.comp &&
            bola.y - bola.raio <= block.y &&
            bola.x >= block.x && 
            bola.x <= block.x + block.comp ) {
                bola.vel_y = -bola.vel_y;
                block.ativo = false;
                        blocos++;
                cout << "BOLA" << endl;
                bola.print();
                
                cout << "BLOCK" <<endl;
                block.print();
            return;
        }
        // Colisao lado DIREITO
        if( bola.x - bola.raio <= block.x + block.comp &&
            bola.x - bola.raio >= block.x &&
            bola.y <= block.y && 
            bola.y >= block.y - block.comp ) {
                bola.vel_x = -bola.vel_x;
                block.ativo = false;
                        blocos++;
            cout << "DIREITA" << endl;
            return;
        }
        
        // Colisao lado ESQUERDO
        if( bola.x + bola.raio >= block.x &&
            bola.x + bola.raio <= block.x + block.comp &&
            bola.y <= block.y && 
            bola.y >= block.y - block.comp ) {
                bola.vel_x = -bola.vel_x;
                block.ativo = false;
                        blocos++;
                cout << "ESQUERDA" << endl;
            return;
        }
        
        

    }
}
 
bool inicializar();
 
int main(void)
{
    bool sair = false;
    list<Block> l;
    if (!inicializar())
    {
        return -1;
    }
    int r = 0;
    for( r = 0; r < LARGURA_TELA; r+= 50 )
        l.push_back( Block( 50, r, 100, al_map_rgb(random() % 256, random() % 256, random() % 256) ) );
    for( r = 50; r < LARGURA_TELA-50; r+= 50 )
        l.push_back( Block( 50, r, 150, al_map_rgb(random() % 256, random() % 256, random() % 256) ) );
    for( r = 100; r < LARGURA_TELA-100; r+= 50 )
        l.push_back( Block( 50, r, 200, al_map_rgb(random() % 256, random() % 256, random() % 256) ) );
    for( r = 150; r < LARGURA_TELA-150; r+= 50 )
        l.push_back( Block( 50, r, 250, al_map_rgb(random() % 256, random() % 256, random() % 256) ) );
    
    
    ALLEGRO_FONT *fonte = NULL;
    fonte = al_load_font("/usr/share/fonts/truetype/tlwg/Purisa.ttf", 20, 0);

    Tabua tabua( 180, 10, 290, 4 );
    Bola bola( 10, 290, 460, 3, 3, al_map_rgb(255, 255, 255) );
    

    int tecla = 0;
    
    
    while (!sair){
    
        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.0005);

 
        int tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);
        

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            sair = true;
        }
        
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(evento.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                tecla = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                tecla = 2;
                break;
            case ALLEGRO_KEY_LEFT:
                tecla = 3;
                break;
            case ALLEGRO_KEY_RIGHT:
                tecla = 4;
                break;
            }
        } else
        
        
        if( tecla == 4 )
            tabua.right();
        else if( tecla == 3 )
            tabua.left(); 
            
        for( list<Block>::iterator i = l.begin();
             i != l.end();
             i++ )
            colision_detect( bola, *i );
    
        bola.move( tabua );
        
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
        
        al_draw_textf(fonte, al_map_rgb(255,255,255), 10, 10, ALLEGRO_ALIGN_LEFT, "%d Blocks", blocos);
        
        tabua.draw();
        bola.draw();
        for( list<Block>::iterator i = l.begin();
             i != l.end();
             i++ )
            i->draw();
           
        al_rest( 0.01 );
        if(bola.y > ALTURA_TELA)
        	sair = true;
    }
     
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}
 
bool inicializar()
{
    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar a Allegro.\n");
        return false;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }
    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return false;
    }
 
    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
        return false;
    }
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }
     // Inicializa o add-on para utilização de imagens
    al_init_image_addon();
    al_set_window_title(janela, "Utilizando o Teclado");
   
    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }
 
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
 
    return true;
}
