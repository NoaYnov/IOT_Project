// Variables to use :
// set up your variables (for exemple the parameters from Adafruit or the contact/positive list)

// ------------------------------------------------------------------------------------------------
// Main Functions for our application
// ------------------------------------------------------------------------------------------------

// Variables



void onGetContact(){
  // TODO : Implementing the function that saves the informations of nearby cards (with the right format)
  // WARNING : this function should call the serialize json function and update a contact as
  // a contact to follow after a certain time
  // This function should be in the bluetooth loop and be called when a card is found
}

void onUpdateState(){
  // TODO : Implementing the function that Updates your state from Adafruit and your contacts
  // You should use the deserialize function
  // This function should be called in the adafruit loop when the state is changed in the app
}

void updateState(){
  // TODO : Implementing the function that Updates your state from your contacts
  // if one of your contact to follow is sick, you should be changed to contact case
  // So you should verify if you have the same person in the contact list and positive list
  // You should use the deserialize function
  // This function should be called in the adafruit loop
}

void updateContacts(){
  // TODO : Implementing the function that Updates your contacts
  // the goal of this function is to update the informations of yours contacts to follow
  // This function should be called in the wifi loop
}

// There should be a admin part, where you could setup OTA and Remote Debug,
// at least to ease your tests and the debug part ...
void remoteDebug(){}
