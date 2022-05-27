import it.unimi.dsi.webgraph.*;
import it.unimi.dsi.webgraph.algo.HyperBall;
import it.unimi.dsi.webgraph.algo.NeighbourhoodFunction;

import java.io.FileWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import static org.junit.Assert.assertEquals;

public class HBApp {
    public static final String syntax = "HBApp {stats|conv|enc} <file>";
    
    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println(syntax);
            return;
        }
        String fname = args[1];
        if (args[0].equals("conv")) {
            BVGraph g = BVGraph.load(fname, 0);
            g.store(g,fname);
            g = BVGraph.load(fname, 2);
            ASCIIGraph.store(g, fname);
        } else if (args[0].equals("enc")) {
            try {
                BVGraph.main(new String[]{"-g", "ASCIIGraph", fname, fname});
            } catch (Exception e) {
                System.out.println("Encoding failed with error " + e.toString());
            }
        } else if (args[0].equals("stats")) {
            BVGraph g = BVGraph.load(fname, 0);
            g.store(g,fname);
            g = BVGraph.load(fname, 2);
            System.out.println("number of nodes: " + g.numNodes());

            HyperBall hb = new HyperBall(g, null, 7, null, 1, 0, 0, false, true, true, null, 0);
            hb.init();

            hb.iterate();
            while (hb.modified() != 0) {
                hb.iterate();
            }
            FileWriter out = new FileWriter(fname + ".out");
            for (int i=0; i<g.numNodes(); i++)
                out.write(hb.sumOfInverseDistances[i] + " " + 1.0/hb.sumOfDistances[i] + "\n");
            out.close();
            double sum=0;
            double min=hb.sumOfInverseDistances[0];
            double max=hb.sumOfInverseDistances[0];
            double sumC=0;
            double minC=hb.sumOfDistances[0];
            double maxC=hb.sumOfDistances[0];
            for (int i=0; i<g.numNodes(); i++) {
                sum += hb.sumOfInverseDistances[i];
                if (hb.sumOfInverseDistances[i] < min) min = hb.sumOfInverseDistances[i];
                if (hb.sumOfInverseDistances[i] > max) max = hb.sumOfInverseDistances[i];
                sumC += hb.sumOfDistances[i];
                if (hb.sumOfDistances[i] < minC) minC = hb.sumOfDistances[i];
                if (hb.sumOfDistances[i] > maxC) maxC = hb.sumOfDistances[i];
            }
            System.out.println("min, mean, max Harmonic Centrality:");
            System.out.println(min + ", " + sum/g.numNodes() + ", " + max);
            System.out.println("min, mean, max Closeness Centrality:");
            System.out.println(1.0/minC + ", " + g.numNodes()/sumC + ", " + 1.0/maxC);
            System.exit(0);
        } else {
            System.out.println(syntax);
        }
    }
}
