import java.io.InputStream;

public class PeriodicChecker extends Thread
{
    private InputStream inputStream;

    public PeriodicChecker(InputStream inputStream) {
        this.inputStream = inputStream;
    }

    @Override
    public void run()
    {
        while(true) {
            try {
                UARTCommunicationModel.readUART();
                System.out.println(UARTCommunicationModel.getOutput());
                Thread.sleep(1000);
            } catch (Exception e) {

            }
        }
    }

}