#ifndef INVERSE_V1_H
#define INVERSE_V1_H

  typedef struct s_vector
  {
    float row[4];
  } s_vector;

  typedef struct s_matrix
  {
    s_vector cols[4];
  } s_matrix;


inline float cofactor_ij_v1(const s_matrix * const mat, unsigned int col, unsigned int row)
{
    static const unsigned int sel0[] = { 1,0,0,0 };
    static const unsigned int sel1[] = { 2,2,1,1 };
    static const unsigned int sel2[] = { 3,3,3,2 };
    
    // Let's first define the 3x3 matrix:
    const unsigned	int col0 = sel0[col];
    const unsigned	int col1 = sel1[col];
    const unsigned	int col2 = sel2[col];
    const unsigned	int row0 = sel0[row];
    const unsigned	int row1 = sel1[row];
    const unsigned	int row2 = sel2[row];
    
    // Compute the float sign-mask:
    const unsigned	int signpos  = (col + row) & 1;
    const unsigned	int signmask = signpos << 31;
    
    // Computer the det of the 3x3 matrix:
    // 
    //   [ a b c ]
    //   [ d e f ] = aei - ahf + dhc - dbi + gbf - gec = (aei + dhc + gbf) - (ahf + dbi + gec)
    //   [ g h i ] 
    //
    
    const float		pos_part1 = mat->cols[col0].row[row0] * mat->cols[col1].row[row1] * mat->cols[col2].row[row2]; // aei
    const float		pos_part2 = mat->cols[col0].row[row1] * mat->cols[col1].row[row2] * mat->cols[col2].row[row0]; // dhc
    const float		pos_part3 = mat->cols[col0].row[row2] * mat->cols[col1].row[row0] * mat->cols[col2].row[row1]; // gbf
                                                                                                        
    const float		neg_part1 = mat->cols[col0].row[row0] * mat->cols[col1].row[row2] * mat->cols[col2].row[row1]; // ahf
    const float		neg_part2 = mat->cols[col0].row[row1] * mat->cols[col1].row[row0] * mat->cols[col2].row[row2]; // dbi
    const float		neg_part3 = mat->cols[col0].row[row2] * mat->cols[col1].row[row1] * mat->cols[col2].row[row0]; // gec	
    
    const float		pos_part  = pos_part1 + pos_part2 + pos_part3;
    const float		neg_part  = neg_part1 + neg_part2 + neg_part3;
    const float		det3x3	  = pos_part - neg_part;
    
    // Now let's adjust the sign of the cofactor:
    union
    {
        float f;
        int	  i;
    } cofactor;
    
    cofactor.f  = det3x3;
    cofactor.i ^= signmask;
    
    return cofactor.f;
}


inline void cofactor_v1(s_matrix * const output, const s_matrix * const source)
{
    int col;
    int row;
    
    for ( col = 0 ; col < 4 ; col++ )
    {
        for ( row = 0; row < 4; row++ )
        {
            output->cols[col].row[row] =  cofactor_ij_v1(source, col, row);
        }
    }
}

inline float determinant_v1(const s_matrix * const  source, const s_matrix * const  cofactor)
{
    int col;
    float det = 0.0f;
    for ( col = 0; col < 4; col++ )
        det += source->cols[col].row[0] * cofactor->cols[col].row[0];
    return det;
}

inline void transpose_v1(s_matrix * const  output, const s_matrix * const  source)
{
    int index1 = 0;
    int index2 = 0;
    
    for ( index1 = 0; index1 < 4; index1++ )
    {
        for ( index2 = 0; index2 < 4; index2++ )
        {
            output->cols[index2].row[index1] = source->cols[index1].row[index2];
        }
    }
}

inline void mul_v1(s_matrix * const  output, const s_matrix * const  source, const float factor)
{
    int col;
    int row;
    
    for ( col = 0 ; col < 4 ; col++ )
    {
        for ( row = 0; row < 4; row++ )
        {
            output->cols[col].row[row] =  source->cols[col].row[row] * factor;
        }
    }
}

inline void inverse_v1(s_matrix * const  output, const s_matrix * const  source)
{
    s_matrix cof;
    s_matrix adj;
    float    oodet;
    
    // InvM = (1/det(M)) * Transpose(Cofactor(M))
    
    cofactor_v1(&cof, source);
    oodet = 1.0f / determinant_v1(source, &cof);
    transpose_v1(&adj, &cof);
    mul_v1(output, &adj, oodet);
}

#endif // INVERSE_V1_H

