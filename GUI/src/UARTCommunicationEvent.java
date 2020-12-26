import java.util.EventObject;

public class UARTCommunicationEvent extends EventObject {

    private final String output;

    public UARTCommunicationEvent(UARTCommunicationModel uartCommunicationModel, String output) {
        super(uartCommunicationModel);
        this.output = output;
    }

    public String getOutput() { return output; }

}
