#include <SoftwareSerial.h>
#include <SPI.h>
#include <Pixy2.h>
#include <math.h>


SoftwareSerial myserial(7 , 8); // RX, TX la carte arduino et la carte du robot 
SoftwareSerial bt(3, 4); // RX, TX
Pixy2 pixy; // déclaration de l'objet pixy2
#define pompePin 2  // Broche de contrôle du module de relais ( D2)
bool pompeActivee = false;      // État de la pompe



//changement d'origine de X  
float changeX(float x , float xo ) 
{
    float newX;
    newX = x - xo;
    return newX;
}
//changement d'origine e Y 
float changeY(float y , float yo ) 
{
    float newY;
    newY = y - yo;
    return newY;
}



// la convertion des coordoner cartesienne en coordoner polaire 

float polarT(float x, float y)
{
  float theta = atan2(y,x) * 180/PI;
  if (theta < 0) {
    theta += 360;
  }
  theta = fmod(theta, 180);
  return theta;
}

float polarR(float x,float y)
{
  float r=sqrt(x * x + y * y);
  return r;
}

// Fonction pour calculer la valeur d'impulsion
int calculerValeurImpulsion(int degre) {
    int  valeurImpulsion;
    if (degre >= 0 && degre <= 90) {
        valeurImpulsion = 500 + (1000.0 / 90.0) * degre;
    } else if (degre > 90 && degre <= 180) {
        valeurImpulsion = (1500 + (1000.0 / 90.0) * (degre - 90) )-220;
    } else {
        // Le degré doit être entre 0 et 180
        return -1;
    }
    return valeurImpulsion;
}


//fonction pour déplacer le servomoteur 
void deplacement(int index, int degre ,int vitesse)
{
    int impultion=calculerValeurImpulsion(degre);
    if (impultion!=-1)
    {
      String command = "#" + String(index) + "P" + String(impultion) + "T" + String(vitesse);
      Serial.println("Mouvement moteur : " + command);
      myserial.println(command);
   } 
   else 
    {
      Serial.println("Erreur : Le degré doit être entre 0 et 180");
    }
}

// Fonction pour activer la pompe
void activerPompe() {
  digitalWrite(pompePin, HIGH);
  pompeActivee = true;
}

// Fonction pour désactiver la pompe
void desactiverPompe() {
  digitalWrite(pompePin, LOW);
  pompeActivee = false;
} 



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pixy.init();  // initialiser l'objet pixy 
  
  
 pinMode(pompePin, OUTPUT);
  digitalWrite(pompePin, LOW);
  

  myserial.begin(9600);//initialiser la communication série à une vitesse de 9600 bauds entre la carte arduino et la carte du robot
          deplacement(1,90,3000);//initialisation du servo moteur numero 1 
          deplacement(2,90,3000);  //initialisation du servo moteur numero 2
          deplacement(0,10,3000);//initialisation du servo moteur numero 0
          delay(3000);
bt.begin(9600);



 //delay(3000);
 
  }

  
void loop() { // run over and over


bt.listen();
  if (bt.available()) {
    
    char c=bt.read();
    Serial.write(c);
    pixy.ccc.getBlocks();

 
     if(pixy.ccc.numBlocks) //si ya des objets detected
       {
      //initialisation des servo
       myserial.listen();
       myserial.println("GETSTATE"); 

      // afficher le nombre de couleurs detected 
       Serial.print(pixy.ccc.numBlocks); 
       Serial.println(" couleur(s) détectée(s) ");

         for (int i=0; i<pixy.ccc.numBlocks; i++)
           { 
                  int signature=pixy.ccc.blocks[i].m_signature ;
                  
              //recuperer les coordonnee x et y de la caméra indiqant la position de l'objet 
                  float x=pixy.ccc.blocks[i].m_x ;
                  float y=pixy.ccc.blocks[i].m_y; 
                  Serial.print(" le x avant le changement d'origine est = ");
                  Serial.println(x);
                  Serial.print("Le y avant le changement d'origine est = ");
                  Serial.println(y);

              //difinir l'origine du robot 
                  float xo=134.00;
                  float yo=205.25;

              //changement d'origine pour x et y 
                  float newX=changeX(x,xo);
                  float newY=changeY(y,yo);
                  Serial.print("le x apres le changement d'origine est = ");
            Serial.println(newX);
            Serial.print("Le y apres le changement d'origine est = ");
            Serial.println(newY);

              //transdormer les coordonee cartesienne en coordonee polaire 
                   float theta=polarT(newX,newY);
                   float r=polarR(newX,newY);
                    Serial.print(" le theta est = ");
                    Serial.println(theta);
                    Serial.print(" le r = ");
                    Serial.println(r);

              
               
               //prendre le objets  
              if(signature==1  && c=='r')//color red
                {
                   Serial.println("le signal est 1 , la couleur est rouge!"); 
                   
                   myserial.listen();
                   deplacement(0,theta,3000);
                   deplacement(1,r+180,3000);
                   deplacement(2,140,3000);
                   if (bt.available()) {
                    char commande=bt.read();
                    Serial.write(c);

                          if (commande == 'H') {
                          activerPompe();
                          } else if (commande == 'L') {
                          desactiverPompe();
                          }
                  }

                   delay(3000);
                   
                     
                }
              if(signature==2 && c=='y') //color=yellow 
                {

                         Serial.println("le signal est 3 , la couleur est yellow!"); 

                         myserial.listen();
                         deplacement(0,theta,3000);
                         deplacement(1,r+180,3000);
                         deplacement(2,140,3000);
                          if (bt.available()) {
                    char commande=bt.read();
                    Serial.write(c);

                          if (commande == 'H') {
                          activerPompe();
                          } else if (commande == 'L') {
                          desactiverPompe();
                          }
                  }

                         delay(3000);
                   
                }
              if(signature==5 && c=='b')//color blue 
                {
                        Serial.println("le signal est 5, la couleur est bleu!");  

                        myserial.listen();
                        deplacement(0,theta,3000);
                        deplacement(1,r+180,3000);
                        deplacement(2,140,3000);
                         if (bt.available()) {
                    char commande=bt.read();
                    Serial.write(c);

                          if (commande == 'H') {
                          activerPompe();
                          } else if (commande == 'L') {
                          desactiverPompe();
                          }
                  }

                        delay(3000);
                }
                 
              
             
     }        
   }  
    else
              {
                 Serial.println(" nous n'avons detecter aucune couleur  ");

              }
  }           

    

  }
 
  
  

