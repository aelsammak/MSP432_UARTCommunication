import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortPacketListener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;

public class UARTCommunicationController implements ActionListener {

    private final UARTCommunicationModel uartCommunicationModel;

    public UARTCommunicationController(UARTCommunicationModel uartCommunicationModel) {
        this.uartCommunicationModel = uartCommunicationModel;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if(e.getActionCommand().equals("next")) {
            try {
                uartCommunicationModel.writeUART("N");
            } catch (IOException exception) {
                exception.printStackTrace();
            }
        } else if(e.getActionCommand().equals("prev")) {
            try {
                uartCommunicationModel.writeUART("P");
            } catch (IOException exception) {
                exception.printStackTrace();
            }
        } else {
            uartCommunicationModel.closeCOM();
        }
        uartCommunicationModel.updateView();
    }
}
