__kernel void integral(__global float *A, __global float *C, __global float *D, int colsTotal, int rowsTotal) {


	int i = get_global_id(0);
        float tempa = 0;
        float tempb = 0;

        // Do the operation

        int row = i/colsTotal;
        int col = i%colsTotal;
        
	if(col < colsTotal && row < rowsTotal)
        {
            for(int c=0; c<=col; c++)
            {
                tempa = tempa + A[row*colsTotal + c];
            }
        }
            D[row*colsTotal+col] = tempa;
        if(col < colsTotal && row < rowsTotal)
        {   
            for(int r=0; r<=row; r++)
            {
                tempb = tempb + D[r*colsTotal + col];
            }
         }
         
         C[row*colsTotal+col] = tempa; //D[row*colsTotal+col];
}
