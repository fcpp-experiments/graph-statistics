import it.unimi.dsi.webgraph.*;
import it.unimi.dsi.webgraph.algo.HyperBall;
import it.unimi.dsi.webgraph.algo.NeighbourhoodFunction;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import static org.junit.Assert.assertEquals;

public class HBApp {

    public HBApp() throws IOException {
    }

    public static final String syntax = "HBApp {stats|conv} <file>";
    
    public static void main(String[] args) throws IOException {
	if (args.length < 2)
	    System.out.println(syntax);

	String fname = args[1]; // e.g. "data/cnr-2000-nat"
	
	if (args[0].equals("conv")) { 
	    BVGraph g = BVGraph.load(fname, 0);
	    g.store(g,fname);
	    g = BVGraph.load(fname, 2);	    
	    ASCIIGraph.store(g, fname);
	} else if (args[0].equals("conv")) { 


	    
	    //	    ASCIIGraph.store(g, fname);
	} else if (args[0].equals("stats")) {
		BVGraph g = BVGraph.load(fname, 1);
		System.out.println("number of nodes: " + g.numNodes());

		HyperBall hb = new HyperBall(g, null, 7, null, 1, 0, 0, false, true, true, null, 0);
		hb.init();

		hb.iterate();
		while (hb.modified() != 0) {
		    hb.iterate();
		}
		System.out.println("neighbourhood: " + hb.neighbourhoodFunction.getDouble(hb.neighbourhoodFunction.size()-1));
		for (int i = 0; i < 10; i++) {
		    System.out.println(i + ": " + hb.sumOfDistances[i] + " -- " + hb.sumOfInverseDistances[i]);
		}

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
		System.out.println(minC + ", " + sumC/g.numNodes() + ", " + maxC);
	} else {
	    System.out.println(syntax);
	}
    }
}
