// cris ababei
// this is a sketch to have an Arduino Uno board play the role of
// an CDC-ACM device, which is connected to an STM32 board: NUCLEO-C071RB
// https://www.st.com/en/evaluation-tools/nucleo-c071rb.html 
// description of the two operation modes is inside loop;

///////////////////////////////////////////////////////////////////////////////
// 
// setup
//
///////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
}

///////////////////////////////////////////////////////////////////////////////
// 
// main
//
///////////////////////////////////////////////////////////////////////////////

void loop(void)
{
  // (1) Version 1 of operation:
  // Send "Cristinel" every 3 seconds all the time;
  // If something is received, echo back the received text like so: "Arduino received: ..."
  Serial.println("Cristinel");
  if (Serial.available()) { // Check if data is available
    String receivedString = Serial.readStringUntil('\n'); // Read until newline
    receivedString.trim(); // Remove leading/trailing whitespace
    Serial.print("Arduino CDC-ACM device received: ");
    Serial.println(receivedString); // Print the received string
  }
  delay(3000);

  // (2) Version 2 of operation:
  // Send "Cristinel" every 3 seconds all the time;
  // Then, when it receives '0' from NUCLEO-C071RB, it replies with "Cristinel"
  // and, when it receives '1' from NUCLEO-C071RB, it replies with "Ababei"
  // if (Serial.available() > 0) { // Check if data is available
  //   char incomingByte = Serial.read(); // Read the incoming byte
  //   Serial.print("Arduino  received: "); // Print a message
  //   Serial.println(incomingByte, DEC); // Print the byte in decimal
  //   Serial.print(incomingByte);

  //   if ((incomingByte - 48) == 0) { // ASCII code for digit '0' is \(48\).
  //    Serial.println("  Cristinel");
  //   } else if ((incomingByte - 48) == 1) { // ASCII code for digit '1' is \(49\)
  //    Serial.println("  Ababei");
  //   } else {
  //    // do nothing;
  //   }
  // } 
}
