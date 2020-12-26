import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import com.fazecast.jSerialComm.SerialPort;

public class UARTCommunicationModel {

    private static String output;
    private static ArrayList<UARTCommunicationView> views;
    private static SerialPort comPort;

    public UARTCommunicationModel() {
        comPort = SerialPort.getCommPorts()[0];
        comPort.openPort();
        views = new ArrayList<>();
    }

    public SerialPort getComPort() { return comPort; }

    public void closeCOM() {
        comPort.closePort();
    }

    public void writeUART(String state) throws IOException {
        OutputStream outputStream = comPort.getOutputStream();
        outputStream.write(state.getBytes());
        outputStream.close();
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
        InputStream inputStream = comPort.getInputStream();
        output = handleUARTOutput(inputStream);
        inputStream.close();
    }

    public void addView(UARTCommunicationView u) { views.add(u); }

    public void removeView(UARTCommunicationView u) { views.remove(u); }

    public static void readUART() throws IOException {
        InputStream inputStream = comPort.getInputStream();
        output = handleOUTPUT(inputStream);
        inputStream.close();
    }

    public static String getOutput() { return output; }

    public static String handleOUTPUT(InputStream inputStream) throws IOException {
        return getString(inputStream);
    }

    private static String getString(InputStream inputStream) throws IOException {
        byte[] bytes = inputStream.readNBytes(1);
        String s = new String(bytes, StandardCharsets.UTF_8);
        return switch (s) {
            case "1" -> "LED 1 : OFF || LED 2 : OFF";
            case "2" -> "LED 1 : OFF || LED 2 : ON";
            case "3" -> "LED 1 : ON || LED 2 : OFF";
            case "4" -> "LED 1 : ON || LED 2 : ON";
            default -> "ERROR";
        };
    }


    public String handleUARTOutput(InputStream inputStream) throws IOException {
        return getString(inputStream);
    }

    public void updateView() {
        for(UARTCommunicationView v : views) {
            v.handleUARTCommunicationUpdate(new UARTCommunicationEvent(this, output));
        }
    }

}
