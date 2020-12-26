import javax.swing.*;
import java.awt.*;
import java.io.InputStream;

public class UARTCommunicationFrame extends JFrame implements UARTCommunicationView {

    private static InputStream inputStream;
    private final JLabel currState;

    public UARTCommunicationFrame() {
        super("UART Communication!");//Sets title of window
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); //close and stop the program when x is clicked
        setLayout(new BorderLayout(0, 150));

        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 150, 0));

        // Label that says "Current State"
        JLabel current = new JLabel("<html><div style='text-align: center;'>Current State:</div></html>");
        current.setFont(new Font("Impact", Font.PLAIN, 50));

        // Label that displays Current State
        currState = new JLabel("<html><div style='text-align: center;'>LED 1 : OFF || LED 2 : OFF</div></html>");
        currState.setFont(new Font("Impact", Font.PLAIN, 40));

        // Next State Button
        JButton nextState = new JButton("Next State");
        nextState.setFont(new Font("Impact", Font.PLAIN, 60));
        nextState.setActionCommand("next");

        // Previous State Button
        JButton prevState = new JButton("Previous State");
        prevState.setFont(new Font("Impact", Font.PLAIN, 60));
        prevState.setActionCommand("prev");

        // Close COM Button
        JButton close = new JButton("Close COM");
        close.setFont(new Font("Impact", Font.PLAIN, 60));
        close.setActionCommand("close");

        current.setHorizontalAlignment(current.CENTER);
        current.setVerticalAlignment(current.CENTER);

        currState.setHorizontalAlignment(currState.CENTER);
        currState.setVerticalAlignment(currState.CENTER);

        JPanel panel = new JPanel(new BorderLayout(0, 30));
        panel.add(current, BorderLayout.NORTH);
        panel.add(currState, BorderLayout.SOUTH);

        // Setting button colors
        close.setBackground(new Color(206, 93, 93));
        prevState.setBackground(new Color(123, 220, 73));
        nextState.setBackground(new Color(92, 213, 194));


        buttonPanel.add(prevState);
        buttonPanel.add(nextState);
        buttonPanel.add(close);

        add(panel, BorderLayout.NORTH);
        add(buttonPanel, BorderLayout.CENTER);
        setSize(1500, 600); //set window size
        setVisible(true); //make window visible
        UARTCommunicationModel uartCommunicationModel = new UARTCommunicationModel();
        uartCommunicationModel.addView(this);
        UARTCommunicationController uartCommunicationController = new UARTCommunicationController(uartCommunicationModel);
        nextState.addActionListener(uartCommunicationController);
        prevState.addActionListener(uartCommunicationController);
        inputStream = uartCommunicationModel.getComPort().getInputStream();
    }

    public static void main(String[] args) {
        Thread t = new PeriodicChecker(inputStream);
        t.start();
        new UARTCommunicationFrame();
    }

    @Override
    public void handleUARTCommunicationUpdate(UARTCommunicationEvent e) {
        currState.setText("<html><div style='text-align: center;'>" + e.getOutput() + "</div></html>");
    }
}
