kernel void
ArrayMult( global const float *dA, global const float *dB, global float *dC, global float *dD)
{
	int gid = get_global_id( 0 );

    //Array Mult
    //dC[gid] = dA[gid] * dB[gid] + dD[gid];

	//Uncomment below and comment above equation for Mult-Add
	dC[gid] = dA[gid] * dB[gid] + dD[gid];
}

kernel void
ArrayMultReduce( global const float *dA, global const float *dB, local float *prods, global float *dC)
{
	int gid = get_global_id( 0 );
	int numItems = get_local_size( 0 );     // # work-items per work-group
	int tnum = get_local_id( 0 );           // thread # for this group

	int wgNum = get_group_id( 0 );          // which work-group number this on

	prods[ tnum ] = dA[ gid ] * dB[ gid ]; // multiply 2 arrays together

	//prods now has all the product each thread calculated.. add them up in same step

	for( int offset = 1; offset < numItems; offset *= 2)
	{
	    int mask = 2*offset - 1;
	    barrier( CLK_LOCAL_MEM_FENCE ); // wait for completion

        if( ( tnum & mask ) == 0 )
        {
            prods[ tnum ] += prods[ tnum + offset ];
        }
     }

    barrier( CLK_LOCAL_MEM_FENCE );
    if( tnum == 0 )
        dC[ wgNum ] = prods[ 0 ];
}
